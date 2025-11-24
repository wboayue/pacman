#ifndef ASSET_REGISTRY_H
#define ASSET_REGISTRY_H

#include <cstdlib>
#include <optional>
#include <string>

/**
 * @brief Enumeration of available sound effects in the game.
 */
enum class Sounds {
  kIntro,       ///< Game intro/start sound
  kMunch1,      ///< Pacman eating pellet sound
  kMunch2,      ///< Alternative pellet eating sound
  kPowerPellet, ///< Power pellet consumed sound
  kDeath        ///< Pacman death sound
};

/// Enumeration of sprite assets.
enum class Sprites {
  kPacman,      ///< Pacman sprite sheet
  kBlinky,      ///< Red ghost (Blinky)
  kPinky,       ///< Pink ghost (Pinky)
  kInky,        ///< Cyan ghost (Inky)
  kClyde,       ///< Orange ghost (Clyde)
  kScaredGhost, ///< Frightened ghost sprite
  kGhostEyes,   ///< Ghost eyes (respawning)
  kPellet,      ///< Regular pellet
  kPowerPellet, ///< Power pellet
  kFruits,      ///< Bonus fruit sprites
  kMaze,        ///< Maze background
  kWhiteText    ///< White text font
};

/// Central registry mapping asset enums to file paths.
class AssetRegistry {
public:
  /// @param assetsPath Base directory for asset files
  AssetRegistry(const std::string &assetsPath) : assetsPath{assetsPath} {};

  AssetRegistry()
      : assetsPath{[]() {
          const char *env = std::getenv("ASSET_PATH");
          return env ? env : "../assets";
        }()} {};

  ~AssetRegistry() {};

  /// Returns full path for a sound asset, or nullopt if unknown.
  auto GetSoundPath(Sounds sound) -> std::optional<std::string>;

  /// Returns full path for a sprite asset, or nullopt if unknown.
  auto GetSpritePath(Sprites sprite) -> std::optional<std::string>;

private:
  std::string assetsPath;
};

#endif
