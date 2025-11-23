#include <iostream>

#include "audio-system.h"
#include "constants.h"

#include "SDL.h"
#include "SDL_mixer.h"

// Audio format depends on SDL_mixer macro
constexpr int kAudioFormat = MIX_DEFAULT_FORMAT;

// Static instance for callbacks
AudioSystem* AudioSystem::instance_ = nullptr;

AudioSystem::AudioSystem(AssetManager &assetManager) : assetManager_(assetManager) {
  // Initialize SDL audio subsystem
  if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
    std::cerr << "SDL could not initialize.\n";
    std::cerr << "SDL_Error: " << SDL_GetError() << "\n";
    return;
  }

  // Initialize SDL_mixer
  if (Mix_OpenAudio(kAudioFrequency, kAudioFormat, kAudioChannels, kAudioChunkSize) < 0) {
    std::cerr << "SDL_mixer could not initialize!\n";
    std::cerr << "SDL_mixer Error: " << Mix_GetError() << "\n";
    return;
  }

  // Allocate mixing channels (default is 8, we'll use 16 for better concurrency)
  Mix_AllocateChannels(16);

  // Set up the singleton instance for callbacks
  instance_ = this;

  // Register channel finished callback
  Mix_ChannelFinished(channelFinishedCallback);

  initialized_ = true;
  // Start audio processing thread
  audioThread_ = std::thread(&AudioSystem::processAudioQueue, this);
}

AudioSystem::~AudioSystem() {
  if (!initialized_) {
    return;
  }

  // Stop all playing sounds
  CancelAllSounds();

  {
    std::lock_guard<std::mutex> lock(queueMutex_);
    running_ = false;
  }

  conditionVariable_.notify_one();

  if (audioThread_.joinable()) {
    audioThread_.join();
  }

  // Clear the singleton instance
  instance_ = nullptr;

  Mix_Quit();
  SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

auto AudioSystem::PlaySound(Sound sound, std::optional<int> loop) -> std::pair<SoundHandle, std::future<void>> {
  if (!initialized_) {
    auto future = std::async(std::launch::deferred, []() {});
    return {0, std::move(future)};
  }

  // For kPowerPellet, cancel any existing one before playing the new one
  if (sound == Sound::kPowerPellet) {
    SoundHandle existingHandle = 0;
    {
      std::lock_guard<std::mutex> lock(activeSoundsMutex_);
      auto it = soundTypeToHandle_.find(Sound::kPowerPellet);
      if (it != soundTypeToHandle_.end()) {
        existingHandle = it->second;
      }
    }
    if (existingHandle != 0) {
      CancelSound(existingHandle);
    }
  }

  // Generate unique handle
  SoundHandle handle = nextHandle_.fetch_add(1);

  auto promise = std::make_shared<std::promise<void>>();
  auto future = promise->get_future();

  {
    std::lock_guard<std::mutex> lock(queueMutex_);
    audioQueue_.push({sound, loop, handle, promise});
  }

  conditionVariable_.notify_one();

  return {handle, std::move(future)};
}

auto AudioSystem::processAudioQueue() -> void {
  while (true) {
    AudioRequest request;
    {
      std::unique_lock<std::mutex> lock(queueMutex_);
      conditionVariable_.wait(lock, [this]() { return !audioQueue_.empty() || !running_; });

      if (!running_ && audioQueue_.empty()) {
        return;
      }

      request = audioQueue_.front();
      audioQueue_.pop();
    }

    // Play the sound on any available channel
    Mix_Chunk *sound = assetManager_.GetSound(request.sound);
    if (sound != nullptr) {
      int channel = Mix_PlayChannel(-1, sound, request.loop.value_or(0));

      if (channel != -1) {
        // Successfully started playback, track the sound
        std::lock_guard<std::mutex> lock(activeSoundsMutex_);
        ActiveSound activeSound{channel, request.sound, request.handle, request.loop, request.completion};
        activeSounds_[request.handle] = activeSound;
        channelToHandle_[channel] = request.handle;
        // Track exclusive sounds by type (for sounds like kPowerPellet that should only have one instance)
        if (request.sound == Sound::kPowerPellet) {
          soundTypeToHandle_[request.sound] = request.handle;
        }
      } else {
        // Failed to play (no channels available), signal completion immediately
        std::cerr << "Failed to play sound: no channels available\n";
        request.completion->set_value();
      }
      // Do not free here: AssetManager owns and caches Mix_Chunk
    } else {
      // Failed to load sound, signal completion immediately
      std::cerr << "Failed to load sound\n";
      request.completion->set_value();
    }

    // Periodically clean up finished sounds
    cleanupFinishedSounds();
  }
}

auto AudioSystem::CancelSound(SoundHandle handle) -> void {
  int channel = -1;
  std::shared_ptr<std::promise<void>> completion;

  {
    std::lock_guard<std::mutex> lock(activeSoundsMutex_);

    auto it = activeSounds_.find(handle);
    if (it == activeSounds_.end()) {
      return;
    }

    // Extract info before removing from maps
    channel = it->second.channel;
    completion = it->second.completion;

    // Remove from channel mapping first (so callback won't find it)
    channelToHandle_.erase(channel);

    // Remove from sound type tracking if this was the active instance
    auto typeIt = soundTypeToHandle_.find(it->second.sound);
    if (typeIt != soundTypeToHandle_.end() && typeIt->second == handle) {
      soundTypeToHandle_.erase(typeIt);
    }

    // Remove from active sounds
    activeSounds_.erase(it);
  }
  // Lock released here - safe to call Mix_HaltChannel

  // Stop the channel (may trigger callback, but we've already cleaned up)
  Mix_HaltChannel(channel);

  // Signal completion
  completion->set_value();
}

auto AudioSystem::CancelAllSounds() -> void {
  std::vector<std::shared_ptr<std::promise<void>>> completions;

  {
    std::lock_guard<std::mutex> lock(activeSoundsMutex_);

    // Collect all completion promises before clearing
    for (auto &[handle, activeSound] : activeSounds_) {
      completions.push_back(activeSound.completion);
    }

    // Clear all tracking maps (so callbacks won't find anything)
    activeSounds_.clear();
    channelToHandle_.clear();
    soundTypeToHandle_.clear();
  }
  // Lock released here - safe to call Mix_HaltChannel

  // Halt all channels (may trigger callbacks, but we've already cleaned up)
  Mix_HaltChannel(-1);

  // Signal completion for all sounds
  for (auto &completion : completions) {
    completion->set_value();
  }
}

auto AudioSystem::removeActiveSoundLocked(SoundHandle handle) -> void {
  auto it = activeSounds_.find(handle);
  if (it == activeSounds_.end()) {
    return;
  }

  // Remove from channel mapping
  channelToHandle_.erase(it->second.channel);

  // Remove from sound type tracking if this was the active instance
  auto typeIt = soundTypeToHandle_.find(it->second.sound);
  if (typeIt != soundTypeToHandle_.end() && typeIt->second == handle) {
    soundTypeToHandle_.erase(typeIt);
  }

  // Signal completion
  it->second.completion->set_value();

  // Remove from active sounds
  activeSounds_.erase(it);
}

auto AudioSystem::cleanupFinishedSounds() -> void {
  std::lock_guard<std::mutex> lock(activeSoundsMutex_);

  // Check each active sound to see if its channel has finished playing
  std::vector<SoundHandle> finishedHandles;

  for (auto &[handle, activeSound] : activeSounds_) {
    if (!Mix_Playing(activeSound.channel)) {
      finishedHandles.push_back(handle);
    }
  }

  // Remove finished sounds
  for (SoundHandle handle : finishedHandles) {
    removeActiveSoundLocked(handle);
  }
}

auto AudioSystem::channelFinishedCallback(int channel) -> void {
  if (instance_ == nullptr) {
    return;
  }

  std::lock_guard<std::mutex> lock(instance_->activeSoundsMutex_);

  // Find the handle for this channel
  auto channelIt = instance_->channelToHandle_.find(channel);
  if (channelIt != instance_->channelToHandle_.end()) {
    instance_->removeActiveSoundLocked(channelIt->second);
  }
}
