#include "game-state.h"

auto GameState::NextLevel() -> void {
  pelletsConsumed = 0;
  level += 1;
  levelCompleted = false;
  mode = GhostMode::kChase;
}
