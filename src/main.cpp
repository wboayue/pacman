#include <iostream>

#include "constants.h"
#include "game.h"

// Exit codes
enum class ExitCode { Success = 0, RuntimeError = 1 };

/**
 * Initializes the game and enters the main game loop.
 *
 * @return ExitCode::Success on successful completion
 *         ExitCode::RuntimeError if an unhandled exception occurs
 */
auto main() -> int {
  try {
    auto game = Game{};

    game.Run(kFrameDuration);
    std::cout << "Game has terminated successfully.\n";

    return static_cast<int>(ExitCode::Success);
    ;
  } catch (const std::exception &e) {
    std::cerr << "Unhandled Exception: " << e.what() << std::endl;
    return static_cast<int>(ExitCode::RuntimeError);
  }
}
