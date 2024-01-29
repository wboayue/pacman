#include <iostream>

#include "game.h"

static constexpr std::size_t kFramesPerSecond{60};
static constexpr std::size_t kMilliSecondsPerSecond{1000};

/**
 * Initializes the game and enters the main game loop.
 * 
 * @return returns 0 if the game exits successfully, 1 if there is an initialization failure.
 */
auto main() -> int {
  auto game = Game{};
  if (!game.Ready()) {
    std::cerr << "Game initialization failed.\n";
    return 1;
  }

  game.Run(kMilliSecondsPerSecond / kFramesPerSecond);
  std::cout << "Game has terminated successfully.\n";
  
  return 0;
}