#ifndef ASSET_REGISTRY_H
#define ASSET_REGISTRY_H

#include <string>
#include <optional>

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

enum class Sprites {
  kPacman,
  kBlinky,
  kPinky,
  kInky,
  kClyde,
  kScaredGhost,
  kGhostEyes,
  kPellet,
  kPowerPellet,
  kFruits,
  kMaze,
  kWhiteText
};

class AssetRegistry {
public:
  /// @param assetsPath Base directory for asset files
  AssetRegistry(const std::string &assetsPath) : assetsPath{assetsPath} {};

  ~AssetRegistry() {};

  auto GetSoundPath(Sounds sound) -> std::optional<std::string>;
  auto GetSpritePath(Sprites sprite) -> std::optional<std::string>;

private:
  std::string assetsPath;
};

#endif