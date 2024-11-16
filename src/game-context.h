#ifndef GAME_CONTEXT_H
#define GAME_CONTEXT_H

enum class GhostMode {
  kScatter,
  kChase,
  kScared,
  kReSpawn,
};

struct GameState {
  int score{0};
  int extraLives{2};
  int level{0};
  int pelletsConsumed{0};
  bool levelCompleted{false};
  bool paused{false};

  auto NextLevel() -> void;
  auto Reset() -> void;
};

#endif