#ifndef PACMAN_H
#define PACMAN_H

#include <string>
#include <vector>

#include "SDL.h"

#include "game-state.h"
#include "grid.h"
#include "sprite.h"
#include "vector2.h"

enum Direction {
  kNeutral,
  kNorth,
  kSouth,
  kEast,
  kWest,
};

const float kMaxSpeed = 75.75757625;

class Pacman {
public:
  Pacman(SDL_Renderer *renderer);

  void Update(const float deltaTime, Grid &grid, GameState &state);
  void Render(SDL_Renderer *renderer);
  void ProcessInput(const Uint8 *state);

  Vec2 GetPosition();
  Vec2 GetGridPosition();
  Vec2 NextGridPosition(const Direction &direction);
  Vec2 NextGridPosition();

private:

  void Reset();
  
  Vec2 position;
  Vec2 velocity;
  Direction heading;

  std::unique_ptr<Sprite> sprite;
  SDL_Renderer *renderer;
};

#endif