#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include <string>
#include <unordered_map>

#include "SDL.h"
#include "SDL_mixer.h"

#include "asset-registry.h"
#include "sprite.h"

/// Centralized resource loader for game assets (sounds, sprites, images).
class AssetManager {
public:
  /// @param assetsPath Base directory for asset files
  AssetManager(const std::string &assetsPath) : registry{assetsPath} {};

  /// Destructor that cleans up cached sounds.
  ~AssetManager();

  /// Loads a sound effect by Sound enum. Returns cached sound or loads and caches it.
  auto GetSound(Sounds sound) -> Mix_Chunk *;

  static auto LoadTexture(SDL_Renderer *renderer, Sprites sprite) -> SDL_Texture *;

private:
  /// Loads a sound effect by asset path. Returns cached sound or loads and caches it.
  auto getSound(const std::string &asset) -> Mix_Chunk *;

  AssetRegistry registry;
  std::unordered_map<std::string, Mix_Chunk *> soundCache;
};

#endif
