#ifndef GAME_H
#define GAME_H

#include <random>
#include <string>
#include <vector>

#include "SDL.h"

#include "asset-manager.h"
#include "audio-system.h"
#include "board-manager.h"
#include "game-context.h"
#include "ghost.h"
#include "grid.h"
#include "pacman.h"
#include "pellet.h"
#include "renderer.h"

/// Main game orchestrator managing the game loop, entities, and subsystems.
/// Uses a state machine pattern (Ready, Play, Paused, Dying, LevelComplete).
class Game {
public:
  /// Initializes SDL, renderer, and all game entities.
  Game(AssetManager &assetManager);

  Game(const Game &) = delete;
  Game &operator=(const Game &) = delete;

  /// Cleans up SDL resources.
  ~Game();

  /// Runs the main game loop (input -> update -> render).
  /// @param targetFrameDuration Target frame duration in milliseconds
  auto Run(std::size_t targetFrameDuration) -> void;

  /// Returns the current score.
  auto GetScore() const -> int;

  /// Returns true if initialization succeeded.
  auto Ready() const -> bool;

  /// Loads a texture from file.
  auto GetTexture(const std::string &fileName) const -> SDL_Texture *;

  friend struct ReadyState;
  friend struct PlayState;
  friend struct PausedState;
  friend struct DyingState;
  friend struct LevelCompleteState;

  /// Pauses all entity animations.
  auto Pause() -> void;

  /// Resumes all entity animations.
  auto Resume() -> void;

  /// Plays a sound effect asynchronously.
  auto PlaySound(Sounds sound) -> void;

private:
  const Uint8 *processInput();
  void update(const float deltaTime);
  void render();

  void createGhosts(SDL_Renderer *renderer);

  int score{0}; // game score

  bool ready_{false};   // initialization flag
  bool running_{false}; // running flag
  std::shared_ptr<Renderer> renderer_;
  Uint32 ticks_count_{0};

  std::unique_ptr<Pacman> pacman;
  Grid grid{};
  std::unique_ptr<BoardManager> board;
  std::vector<std::shared_ptr<Ghost>> ghosts;
  std::shared_ptr<Ghost> blinky;
  GameContext context{};

  AssetManager &assetManager;
  AudioSystem audio;
};

#endif
