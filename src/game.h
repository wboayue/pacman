#ifndef GAME_H
#define GAME_H

#include <random>
#include <string>

#include "SDL.h"

#include "renderer.h"
#include "pacman.h"
#include "grid.h"
#include "pellet.h"
#include "power-pellet.h"
#include "board-manager.h"

class Game {
public:
  Game();
  ~Game();

  void Run(std::size_t target_frame_duration);
  int GetScore() const;
  bool Ready() const;

  SDL_Texture* GetTexture(std::string fileName);

private:
  std::random_device dev;
  std::mt19937 engine;
  std::uniform_int_distribution<int> random_w;
  std::uniform_int_distribution<int> random_h;

  int score{0};

  void ProcessInput();
  void Update(const float deltaTime);
  void Render();

  bool ready_;
  bool running_;
  std::shared_ptr<Renderer> renderer_;
  Uint32 ticks_count_;

  std::unique_ptr<Pacman> pacman;
  Grid grid;
  std::vector<std::unique_ptr<PowerPellet>> powerPellets;
  std::vector<std::unique_ptr<Pellet>> pellets;
  std::unique_ptr<BoardManager> board;
};

#endif