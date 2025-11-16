#include <iostream>

#include "game.h"

// Exit codes
enum class ExitCode { Success = 0, RuntimeError = 1 };

// Game configuration
struct GameConfig {
  static constexpr std::size_t kFramesPerSecond{60};
  static constexpr std::size_t kMilliSecondsPerSecond{1000};
  static constexpr std::size_t kFrameDuration = kMilliSecondsPerSecond / kFramesPerSecond;
};

/**
 * Initializes the game and enters the main game loop.
 *
 * @return ExitCode::Success on successful completion
 *         ExitCode::RuntimeError if an unhandled exception occurs
 */
auto main() -> int {
  try {
    auto game = Game{};

    game.Run(GameConfig::kFrameDuration);
    std::cout << "Game has terminated successfully.\n";

    return static_cast<int>(ExitCode::Success);
    ;
  } catch (const std::exception &e) {
    std::cerr << "Unhandled Exception: " << e.what() << std::endl;
    return static_cast<int>(ExitCode::RuntimeError);
  }
}
