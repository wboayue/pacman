#include <iostream>

#include "audio-system.h"
#include "constants.h"

#include "SDL.h"
#include "SDL_mixer.h"

// Audio format depends on SDL_mixer macro
constexpr int kAudioFormat = MIX_DEFAULT_FORMAT;

AudioSystem::AudioSystem() {
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

  initialized_ = true;
  // Start audio processing thread
  audioThread_ = std::thread(&AudioSystem::processAudioQueue, this);
}

AudioSystem::~AudioSystem() {
  if (!initialized_) {
    return;
  }

  {
    std::lock_guard<std::mutex> lock(queueMutex_);
    running_ = false;
  }

  conditionVariable_.notify_one();

  if (audioThread_.joinable()) {
    audioThread_.join();
  }

  Mix_Quit();
  SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

auto AudioSystem::PlaySound(Sound sound, std::optional<int> loop) -> std::future<void> {
  if (!initialized_) {
    return std::async(std::launch::deferred, []() {});
  }

  auto promise = std::make_shared<std::promise<void>>();
  auto future = promise->get_future();

  {
    std::lock_guard<std::mutex> lock(queueMutex_);
    audioQueue_.push({sound, loop, promise});
  }

  conditionVariable_.notify_one();

  return future;
}

/**
 * @brief Maps Sound enum to corresponding audio file path
 *
 * @param sound The sound effect to map
 * @return std::string Path to the sound file
 */
auto getSoundFile(Sound sound) -> std::string {
  switch (sound) {
  case Sound::kMunch1:
    return "../assets/sounds/munch_1.wav";
  case Sound::kPowerPellet:
    return "../assets/sounds/power_pellet.wav";
  case Sound::kDeath:
    return "../assets/sounds/death_1.wav";
  default:
    return "../assets/sounds/game_start.wav";
  }
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

    // Play the sound
    std::string soundFile = getSoundFile(request.sound);
    Mix_Chunk *sound = Mix_LoadWAV(soundFile.c_str());
    if (sound != nullptr) {
      Mix_PlayChannel(-1, sound, request.loop.value_or(0));
      while (Mix_Playing(-1) != 0) {
        SDL_Delay(100);
      }
      Mix_FreeChunk(sound);
    }

    // Signal completion
    request.completion->set_value();
  }
}
