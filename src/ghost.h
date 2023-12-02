#ifndef GHOST_H
#define GHOST_H

#include "game-state.h"
#include "grid.h"
#include "sprite.h"
#include "vector2.h"

class Ghost {
public:
  Ghost(std::unique_ptr<Sprite> sprite, Vec2 position);

  void Update(const float deltaTime, Grid &grid, GameState &state);
  void Render(SDL_Renderer *renderer);

private:
  
  Vec2 position;
  Vec2 velocity;
//  Direction heading;

  std::unique_ptr<Sprite> sprite;
};

  // sprite = std::make_unique<Sprite>(renderer, "../assets/pacman.png", 8, 16);
  // sprite->SetFrames({1, 2});

#endif