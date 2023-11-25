#include <iostream>

#include "game.h"

int main() {
  constexpr std::size_t kFramesPerSecond{60};

  Game game;
  if (game.Ready()) {
    game.Run(kFramesPerSecond);
    std::cout << "Game has terminated successfully!\n";
    return 0;
  }

  return 1;
}