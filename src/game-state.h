#ifndef GAME_STATE_H
#define GAME_STATE_H

enum class GhostMode {
  kScatter,
  kChase,
};

struct GameState {
  int score;
  int extraLives;
  int level;
  int pelletsConsumed;
  bool levelCompleted;
  GhostMode mode;
};

#endif