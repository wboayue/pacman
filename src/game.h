#ifndef GAME_H
#define GAME_H

#include <random>
#include <string>
#include <vector>

#include "SDL.h"

#include "audio-system.h"
#include "board-manager.h"
#include "game-state.h"
#include "ghost.h"
#include "grid.h"
#include "pacman.h"
#include "pellet.h"
#include "renderer.h"

class Game {
public:
  Game();
  ~Game();

  auto Run(std::size_t target_frame_duration) -> void;
  auto GetScore() const -> int;
  auto Ready() const -> bool;

  auto GetTexture(const std::string &fileName) const -> SDL_Texture *;

private:
  void processInput();
  void update(const float deltaTime);
  void render();

  void createGhosts(SDL_Renderer *renderer);

  std::random_device dev{};
  std::mt19937 engine{};
  std::uniform_int_distribution<int> random_w{};
  std::uniform_int_distribution<int> random_h{};

  int score{0};

  bool ready_{false};
  bool running_{false};
  std::shared_ptr<Renderer> renderer_;
  Uint32 ticks_count_{0};

  std::unique_ptr<Pacman> pacman;
  Grid grid{};
  std::unique_ptr<BoardManager> board;
  std::vector<std::shared_ptr<Ghost>> ghosts;
  std::shared_ptr<Ghost> blinky;
  GameState state{};
  AudioSystem audio{};
};

#endif