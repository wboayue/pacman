#include <iostream>

#include "audio-system.h"

#include "SDL.h"
#include "SDL_mixer.h"

AudioSystem::AudioSystem() : enabled{false} {
  // Initialize SDL
  if (SDL_Init(SDL_INIT_AUDIO) < 0) {
    std::cerr << "SDL could not initialize.\n";
    std::cerr << "SDL_Error: " << SDL_GetError() << "\n";
    return;
  }

  // Initialize SDL_mixer
  // if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
  //     printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
  //     return;
  // }

  enabled = true;
}

// SDL_InitSubSystem
// SDL_QuitSubSystem(Uint32 flags)
void AudioSystem::PlaySync(Sound sound) {
  if (!enabled) {
    return;
  }

  // Mix_Chunk* sound = Mix_LoadWAV("your_sound_file.wav");
  //  Mix_LoadWa
  //   if (sound == nullptr) {
  //     printf("Failed to load sound file! SDL_mixer Error: %s\n", Mix_GetError());
  //     return 1;
  //   }
  // }
}

void AudioSystem::PlayAsync(Sound sound) {
  if (!enabled) {
    return;
  }
}
