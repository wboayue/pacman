#include "pellet.h"

Pellet::Pellet(SDL_Renderer *renderer, const Vec2 position)
    :sprite{renderer, "../assets/pellet.png"}, position{position}
{
//  sprite.SetFrames({1, 2});
}

void Pellet::Update(const float deltaTime)
{
    sprite.Update(deltaTime);
}

void Pellet::Render(SDL_Renderer *renderer)
{
    sprite.Render(renderer, position.x*8, position.y*8);
}
