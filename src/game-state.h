#ifndef GAME_STATE_H
#define GAME_STATE_H

struct GameState {
  int score;
  int extraLives;
  int level;
  int pelletsConsumed;
  bool levelCompleted;
};

#endif