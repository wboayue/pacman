#include "pellet.h"

Pellet::Pellet(SDL_Renderer *renderer, const Vec2 position)
    : Pellet(renderer, position, false)
{
}

Pellet::Pellet(SDL_Renderer *renderer, const Vec2 position, bool power)
    : position{position}
{
    if (power) {
        sprite = std::make_unique<Sprite>(renderer, "../assets/power-pellet.png", 3, 8);
        sprite->SetFrames({1, 2});
    } else {
        sprite = std::make_unique<Sprite>(renderer, "../assets/pellet.png");
    }
}

void Pellet::Update(const float deltaTime)
{
    sprite->Update(deltaTime);
}

void Pellet::Render(SDL_Renderer *renderer)
{
    sprite->Render(renderer, position.x*8, position.y*8);
}
