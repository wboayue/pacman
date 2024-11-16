#include "game-context.h"

auto GameState::NextLevel() -> void {
  pelletsConsumed = 0;
  level += 1;
  levelCompleted = false;
}

auto GameState::Reset() -> void {
  score = 0;
  extraLives = 2;
  level = 0;
  pelletsConsumed = 0;
  levelCompleted = false;
}
