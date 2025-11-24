#include <string>
#include <optional>
#include <unordered_map>

#include "asset-registry.h"

auto AssetRegistry::GetSoundPath(Sounds sound) -> std::optional<std::string> {
  static const std::unordered_map<Sounds, std::string> soundMap = {
    { Sounds::kIntro,      "/sounds/game_start.wav" },
    { Sounds::kMunch1,     "/sounds/munch_1.wav" },
    { Sounds::kMunch2,     "/sounds/munch_2.wav" },
    { Sounds::kPowerPellet,"/sounds/power_pellet.wav" },
    { Sounds::kDeath,      "/sounds/death_1.wav" }
  };

  auto it = soundMap.find(sound);
  if (it != soundMap.end()) {
    return assetsPath + it->second;
  }

  return std::nullopt;
}

auto AssetRegistry::GetSpritePath(Sprites sprite) -> std::optional<std::string> {
  static const std::unordered_map<Sprites, std::string> spriteMap = {
    { Sprites::kPacman,     "/sprites/pacman.png" },
    { Sprites::kBlinky,     "/sprites/blinky.png" },
    { Sprites::kPinky,      "/sprites/pinky.png" },
    { Sprites::kInky,       "/sprites/inky.png" },
    { Sprites::kClyde,      "/sprites/clyde.png" },
    { Sprites::kScaredGhost,"/sprites/scared-ghost.png" },
    { Sprites::kGhostEyes,  "/sprites/ghost-eyes.png" },
    { Sprites::kPellet,     "/sprites/pellet.png" },
    { Sprites::kPowerPellet,"/sprites/power-pellet.png" },
    { Sprites::kFruits,     "/sprites/fruits.png" },
    { Sprites::kMaze,       "/sprites/maze.png" },
    { Sprites::kWhiteText,  "/sprites/white-text.png" }
  };

  auto it = spriteMap.find(sprite);
  if (it != spriteMap.end()) {
    return assetsPath + it->second;
  }

  return std::nullopt;
}
