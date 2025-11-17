#include <iostream>

#include "asset-manager.h"

auto AssetManager::GetSound(const std::string &asset) -> Mix_Chunk * {
  auto sound = Mix_LoadWAV(asset.c_str());
  if (sound == nullptr) {
    std::cerr << "Failed to load sound: " << asset << "\n";
  }
  return sound;
}

auto AssetManager::GetSound(Sound sound) -> Mix_Chunk * {
  std::string soundFile;
  switch (sound) {
  case Sound::kMunch1:
    soundFile = assetsPath + "/sounds/munch_1.wav";
    break;
  case Sound::kPowerPellet:
    soundFile = assetsPath + "/sounds/power_pellet.wav";
    break;
  case Sound::kDeath:
    soundFile = assetsPath + "/sounds/death_1.wav";
    break;
  default:
    soundFile = assetsPath + "/sounds/game_start.wav";
    break;
  }
  return GetSound(soundFile);
}

auto AssetManager::CreateSprite(const std::string &asset, int frameWidth, int fps) -> Sprite {
  return Sprite{renderer, asset, fps, frameWidth};
}