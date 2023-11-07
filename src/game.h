#ifndef GAME_H
#define GAME_H

#include "SDL.h"
#include "renderer.h"
#include <random>

class Game {
public:
  Game();
  ~Game();

  void Run(std::size_t target_frame_duration);
  int GetScore() const;
  bool Ready() const;

private:
  std::random_device dev;
  std::mt19937 engine;
  std::uniform_int_distribution<int> random_w;
  std::uniform_int_distribution<int> random_h;

  int score{0};

  void ProcessInput();
  void Update();
  void Render();

  bool ready_;
  bool running_;
  std::shared_ptr<Renderer> renderer_;
};

#endif