#ifndef GAME_H
#define GAME_H

#include <random>
#include <string>
#include <vector>

#include "SDL.h"

#include "board-manager.h"
#include "game-state.h"
#include "grid.h"
#include "pacman.h"
#include "ghost.h"
#include "pellet.h"
#include "renderer.h"

class Game {
public:
  Game();
  ~Game();

  void Run(std::size_t target_frame_duration);
  int GetScore() const;
  bool Ready() const;

  SDL_Texture *GetTexture(std::string fileName);

private:
  std::random_device dev;
  std::mt19937 engine;
  std::uniform_int_distribution<int> random_w;
  std::uniform_int_distribution<int> random_h;

  int score{0};

  void ProcessInput();
  void Update(const float deltaTime);
  void Render();

  void CreateGhosts(SDL_Renderer* renderer);

  bool ready_;
  bool running_;
  std::shared_ptr<Renderer> renderer_;
  Uint32 ticks_count_;

  std::unique_ptr<Pacman> pacman;
  Grid grid;
  std::unique_ptr<BoardManager> board;
  std::vector<std::unique_ptr<Ghost>> ghosts;
  GameState state;
};

#endif