#include <iostream>

#include "game.h"

static constexpr std::size_t kFramesPerSecond{60};
static constexpr std::size_t kMilliSecondsPerSecond{1000};

auto main() -> int {
  auto game = Game{};
  if (!game.Ready()) {
    return 1;
  }

  game.Run(kMilliSecondsPerSecond / kFramesPerSecond);
  std::cout << "Game has terminated successfully!!\n";
  
  return 0;
}