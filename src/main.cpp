#include <iostream>

#include "game.h"

static constexpr std::size_t kFramesPerSecond{60};

auto main() -> int {
  auto game = Game{};
  if (!game.Ready()) {
    return 1;
  }

  game.Run(1000 / kFramesPerSecond);
  std::cout << "Game has terminated successfully!!\n";
  return 0;
}