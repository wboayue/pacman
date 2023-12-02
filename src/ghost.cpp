#include <iostream>

#include "ghost.h"

Ghost::Ghost(std::unique_ptr<Sprite> sprite_, Vec2 position)
  : sprite{std::move(sprite_)}, position{position}
 {
  sprite->SetFrames({0, 1});
}

void Ghost::Update(const float deltaTime, Grid &grid, GameState &state)
{
  sprite->Update(deltaTime);
}

void Ghost::Render(SDL_Renderer *renderer)
{
  sprite->Render(renderer, {floor(position.x - 8), floor(position.y - 8)});
}
