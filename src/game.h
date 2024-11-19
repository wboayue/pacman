#ifndef GAME_H
#define GAME_H

#include <random>
#include <string>
#include <vector>

#include "SDL.h"

#include "audio-system.h"
#include "board-manager.h"
#include "game-context.h"
#include "ghost.h"
#include "grid.h"
#include "pacman.h"
#include "pellet.h"
#include "renderer.h"

/**
 * The Game class manages game entities and runs the game loop.
 */
class Game {
public:
  /**
   * Initializes the game.
   */
  Game();

  /**
   * Cleans up allocated resources.
   */
  ~Game();

  /**
   * Runs the game loop. The game loop consists of:
   * 1. getting user input
   * 2. updating game entities
   * 3. rendering the game entities
   * 
   * @param target_frame_duration target duration for each frame 
   */
  auto Run(std::size_t targetFrameDuration) -> void;

  /**
   * Gets the current score.
   * 
   * @return current score 
   */
  auto GetScore() const -> int;

  /**
   * Checks if game initialization was successful.
   * 
   * @return true if game initialized successfully, else false. 
   */
  auto Ready() const -> bool;

  /**
   * Loads a SDL_Texture from a file.
   * 
   * @param fileName file to load texture from.
   * 
   * @return SDL_Texture* 
   */
  auto GetTexture(const std::string &fileName) const -> SDL_Texture *;

  friend struct ReadyState;
  friend struct PlayState;
  friend struct PausedState;
  friend struct DyingState;
  friend struct LevelCompleteState;

  auto Pause() -> void;
  auto Resume() -> void;
  auto PlaySound(Sound sound) -> void;

private:
  const Uint8 * processInput();
  void update(const float deltaTime);
  void render();

  void createGhosts(SDL_Renderer *renderer);

  int score{0}; // game score

  bool ready_{false}; // initialization flag
  bool running_{false}; // running flag
  std::shared_ptr<Renderer> renderer_;
  Uint32 ticks_count_{0};

  std::unique_ptr<Pacman> pacman;
  Grid grid{};
  std::unique_ptr<BoardManager> board;
  std::vector<std::shared_ptr<Ghost>> ghosts;
  std::shared_ptr<Ghost> blinky;
  GameContext context{};
  AudioSystem audio{};
};

#endif