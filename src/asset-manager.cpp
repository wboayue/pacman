#include <iostream>

#include "asset-manager.h"

auto AssetManager::GetSound(const std::string &asset) -> Mix_Chunk* {
  auto sound = Mix_LoadWAV(asset.c_str());
  if (sound == nullptr) {
    std::cerr << "Failed to load sound: " << asset << "\n";
  }
  return sound;
}

auto AssetManager::CreateSprite(const std::string &asset, int frameWidth, int fps) -> Sprite {
  return Sprite{renderer, asset, fps, frameWidth};
}