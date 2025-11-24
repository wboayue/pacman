#include <iostream>
#include <stdexcept>
#include <string_view>

#include "SDL.h"
#include "SDL_image.h"

#include "asset-manager.h"

AssetManager::~AssetManager() {
  for (auto &[path, chunk] : soundCache) {
    if (chunk != nullptr) {
      Mix_FreeChunk(chunk);
    }
  }
  soundCache.clear();
}

auto LoadTexture(SDL_Renderer *renderer, std::string fileName) -> SDL_Texture *;

static AssetRegistry Registry;

auto AssetManager::LoadSpriteTexture(SDL_Renderer *renderer, Sprites sprite) -> SDL_Texture * {
  const auto assetPath = Registry.GetSpritePath(sprite);
  if (!assetPath.has_value()) {
    throw std::runtime_error("Unknown sprite enum");
  }

  return LoadTexture(renderer, *assetPath);
}

auto AssetManager::getSound(const std::string &asset) -> Mix_Chunk * {
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

constexpr auto toString(Sounds sound) -> std::string_view {
  switch (sound) {
  case Sounds::kIntro:
    return "Intro";
  case Sounds::kMunch1:
    return "Munch1";
  case Sounds::kMunch2:
    return "Munch2";
  case Sounds::kPowerPellet:
    return "PowerPellet";
  case Sounds::kDeath:
    return "Death";
  }
  return "UnknownSound";
}

auto AssetManager::GetSound(Sounds sound) -> Mix_Chunk * {
  std::optional<std::string> soundPath = registry.GetSoundPath(sound);

  if (!soundPath.has_value()) {
    throw std::runtime_error("Unknown sound enum: " + std::string(toString(sound)));
  }

  return getSound(*soundPath);
}

auto LoadTexture(SDL_Renderer *renderer, std::string fileName) -> SDL_Texture * {
  SDL_Surface *surface = IMG_Load(fileName.c_str());
  if (!surface) {
    SDL_Log("Failed to load texture file %s", fileName.c_str());
    std::abort();
    return nullptr;
  }

  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_assert(texture != nullptr);
  SDL_FreeSurface(surface);

  return texture;
}
