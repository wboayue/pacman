#include "pacman.h"

Pacman::Pacman(SDL_Renderer *renderer)
    : speed{1, 0}
{
  sprite = std::make_unique<Sprite>(renderer, "../assets/pacman.png", 4, 16);
  sprite->SetFrames({1, 2});
}

void Pacman::Update(const float deltaTime)
{
    position += speed;

    if (position.x > 200) {
        position.x = 0;
    }
    if (position.y > 250) {
        position.y = 0;
    }

    sprite->Update(deltaTime);
}

void Pacman::Render(SDL_Renderer *renderer)
{
    sprite->Render(renderer, position.x, position.y);
}

void Pacman::ProcessInput(const Uint8 *state)
{
    if (state[SDL_SCANCODE_RIGHT]) {
        sprite->SetFrames({1, 2});
        speed = Vector2<int>{2, 0};
    } else if (state[SDL_SCANCODE_LEFT]) {
        sprite->SetFrames({3, 4});
        speed = Vector2<int>{-2, 0};
    } else if (state[SDL_SCANCODE_UP]) {
        sprite->SetFrames({5, 6});
        speed = Vector2<int>{0, -2};
    } else if (state[SDL_SCANCODE_DOWN]) {
        sprite->SetFrames({7, 8});
        speed = Vector2<int>{0, 3};
    }
}
