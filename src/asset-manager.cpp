#include <iostream>

#include "asset-manager.h"

AssetManager::~AssetManager() {
  for (auto &[path, chunk] : soundCache) {
    if (chunk != nullptr) {
      Mix_FreeChunk(chunk);
    }
  }
  soundCache.clear();
}

auto AssetManager::GetSound(const std::string &asset) -> Mix_Chunk * {
  // Check cache first
  auto it = soundCache.find(asset);
  if (it != soundCache.end()) {
    return it->second;
  }

  // Load and cache the sound
  auto sound = Mix_LoadWAV(asset.c_str());
  if (sound == nullptr) {
    std::cerr << "Failed to load sound: " << asset << "\n";
  } else {
    soundCache[asset] = sound;
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