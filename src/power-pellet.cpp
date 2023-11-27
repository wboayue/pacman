#include "power-pellet.h"

PowerPellet::PowerPellet(SDL_Renderer *renderer, const Vec2 position)
    :sprite{renderer, "../assets/power-pellet.png", 3, 8}, position{position}
{
  sprite.SetFrames({1, 2});
}

void PowerPellet::Update(const float deltaTime)
{
    sprite.Update(deltaTime);
}

void PowerPellet::Render(SDL_Renderer *renderer)
{
    sprite.Render(renderer, position.x*8, position.y*8);
}
