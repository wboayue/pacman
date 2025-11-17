#include "game-context.h"
#include "constants.h"

auto GameContext::NextLevel() -> void {
  pelletsConsumed = 0;
  level += 1;
}

auto GameContext::Reset() -> void {
  score = 0;
  extraLives = 2;
  level = 0;
  pelletsConsumed = 0;
}

auto GameContext::LevelComplete() -> bool { return pelletsConsumed >= kTotalPellets; }
