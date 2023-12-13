#include <iostream>

#include "audio-system.h"

#include "SDL.h"
#include "SDL_mixer.h"

AudioSystem::AudioSystem() {
  if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
    std::cerr << "SDL could not initialize.\n";
    std::cerr << "SDL_Error: " << SDL_GetError() << "\n";
    return;
  }

  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
    std::cerr << "SDL_mixer could not initialize!\n";
    std::cerr << "SDL_mixer Error: " << Mix_GetError() << "\n";
    return;
  }

  enabled_ = true;
}

AudioSystem::~AudioSystem() {
  if (!enabled_) {
    return;
  }

  Mix_Quit();

  SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void AudioSystem::PlaySync(Sound sound_) {
  if (!enabled_) {
    return;
  }

  std::string soundFile{"../assets/sounds/game_start.wav"};
  if (sound_ == Sound::kDeath) {
    soundFile = "../assets/sounds/death_1.wav";
  }

  // Load a sound file
  Mix_Chunk *sound = Mix_LoadWAV(soundFile.c_str());
  if (sound == nullptr) {
    printf("Failed to load sound file! SDL_mixer Error: %s\n", Mix_GetError());
    return;
  }

  // Play the sound synchronously
  if (Mix_PlayChannelTimed(-1, sound, 0, -1) == -1) {
    printf("Failed to play sound! SDL_mixer Error: %s\n", Mix_GetError());
  }

  // Wait for the sound to finish playing
  while (Mix_Playing(-1) != 0) {
    SDL_Delay(100); // Add a short delay to reduce CPU usage
  }

  // Free the loaded sound
  Mix_FreeChunk(sound);
}

//   void PlaySync(Sound sound, std::function<void()> callback);
void AudioSystem::PlaySync(Sound sound_, std::function<void()> callback) {
  if (!enabled_) {
    return;
  }

  const std::string soundFile{"../assets/sounds/game_start.wav"};

  // Load a sound file
  Mix_Chunk *sound = Mix_LoadWAV(soundFile.c_str());
  if (sound == nullptr) {
    printf("Failed to load sound file! SDL_mixer Error: %s\n", Mix_GetError());
    return;
  }

  // Play the sound synchronously
  if (Mix_PlayChannelTimed(-1, sound, 0, -1) == -1) {
    printf("Failed to play sound! SDL_mixer Error: %s\n", Mix_GetError());
  }

  // Wait for the sound to finish playing
  while (Mix_Playing(-1) != 0) {
    callback();
    SDL_Delay(100); // Add a short delay to reduce CPU usage
  }

  // Free the loaded sound
  Mix_FreeChunk(sound);
}

void AudioSystem::PlayAsync(Sound sound_) {
  if (!enabled_) {
    return;
  }

  std::string soundFile;
  if (sound_ == Sound::kMunch1) {
    soundFile = "../assets/sounds/munch_1.wav";
  } else if (sound_ == Sound::kPowerPellet) {
    soundFile = "../assets/sounds/power_pellet.wav";
  } else {
    soundFile = "../assets/sounds/game_start.wav";
  }

  // Load a sound file
  Mix_Chunk *sound = Mix_LoadWAV(soundFile.c_str());
  if (sound == nullptr) {
    printf("Failed to load sound file! SDL_mixer Error: %s\n", Mix_GetError());
    return;
  }

  // Play the sound synchronously
  if (Mix_PlayChannel(-1, sound, 0) == -1) {
    printf("Failed to play sound! SDL_mixer Error: %s\n", Mix_GetError());
  }

  // Free the loaded sound
  //  Mix_FreeChunk(sound);
}

void AudioSystem::PlayAsync(Sound sound_, int loop) {
  if (!enabled_) {
    return;
  }

  std::string soundFile;
  if (sound_ == Sound::kMunch1) {
    soundFile = "../assets/sounds/munch_1.wav";
  } else if (sound_ == Sound::kPowerPellet) {
    soundFile = "../assets/sounds/power_pellet.wav";
  } else {
    soundFile = "../assets/sounds/game_start.wav";
  }

  // Load a sound file
  Mix_Chunk *sound = Mix_LoadWAV(soundFile.c_str());
  if (sound == nullptr) {
    printf("Failed to load sound file! SDL_mixer Error: %s\n", Mix_GetError());
    return;
  }

  // Play the sound synchronously
  if (Mix_PlayChannel(-1, sound, loop) == -1) {
    printf("Failed to play sound! SDL_mixer Error: %s\n", Mix_GetError());
  }

  // Mix_HaltChannel(-1);

  // Free the loaded sound
  //  Mix_FreeChunk(sound);
}

auto playSound(const std::string &soundFile) -> int {
  // Initialize SDL
  if (SDL_Init(SDL_INIT_AUDIO) < 0) {
    printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    return 1;
  }

  // Initialize SDL_mixer
  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
    printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
    return 1;
  }

  // Load a sound file
  Mix_Chunk *sound = Mix_LoadWAV(soundFile.c_str());
  if (sound == nullptr) {
    printf("Failed to load sound file! SDL_mixer Error: %s\n", Mix_GetError());
    return 1;
  }

  // Play the sound synchronously
  if (Mix_PlayChannelTimed(-1, sound, 0, -1) == -1) {
    printf("Failed to play sound! SDL_mixer Error: %s\n", Mix_GetError());
  }

  // Wait for the sound to finish playing
  while (Mix_Playing(-1) != 0) {
    SDL_Delay(100); // Add a short delay to reduce CPU usage
  }

  // Free the loaded sound
  Mix_FreeChunk(sound);

  // // Quit SDL_mixer
  // Mix_Quit();

  // // Quit SDL
  // SDL_Quit();

  return 0;
}