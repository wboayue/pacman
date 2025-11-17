#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include <string>
#include <unordered_map>

#include "SDL.h"
#include "SDL_mixer.h"

#include "sprite.h"

/**
 * @brief Enumeration of available sound effects in the game.
 */
enum class Sound {
  kIntro,       ///< Game intro/start sound
  kMunch1,      ///< Pacman eating pellet sound
  kMunch2,      ///< Alternative pellet eating sound
  kPowerPellet, ///< Power pellet consumed sound
  kDeath        ///< Pacman death sound
};

/// Centralized resource loader for game assets (sounds, sprites, images).
class AssetManager {
public:
  /// @param assetsPath Base directory for asset files
  AssetManager(const std::string &assetsPath) : assetsPath{assetsPath} {};

  /// Destructor that cleans up cached sounds.
  ~AssetManager();

  /// Loads a sound effect by asset path. Returns cached sound or loads and caches it.
  auto GetSound(const std::string &asset) -> Mix_Chunk *;

  /// Loads a sound effect by Sound enum. Returns cached sound or loads and caches it.
  auto GetSound(Sound sound) -> Mix_Chunk *;

  /// Creates an animated sprite from a sprite sheet.
  auto CreateSprite(const std::string &asset, int frameWidth, int fps) -> Sprite;

  /// Returns full path to an image asset.
  auto GetImagePath(const std::string &asset) -> std::string { return assetsPath + asset; }

  /// Alias for GetImagePath.
  auto GetImage(const std::string &asset) -> std::string { return assetsPath + asset; }

private:
  std::string assetsPath;
  SDL_Renderer *renderer;
  std::unordered_map<std::string, Mix_Chunk *> soundCache;
};

#endif
