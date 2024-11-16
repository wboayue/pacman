#ifndef GAME_CONTEXT_H
#define GAME_CONTEXT_H

enum class GhostMode {
  kScatter,
  kChase,
  kScared,
  kReSpawn,
};

struct GameContext {
  int score{0};
  int extraLives{2};
  int level{0};
  int pelletsConsumed{0};
  bool paused{false};

  // pacman, ghosts
  // audio system

  auto NextLevel() -> void;
  auto Reset() -> void;
  auto LevelComplete() -> bool;
};

#endif