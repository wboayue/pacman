#include "pacman.h"

Pacman::Pacman(SDL_Renderer *renderer)
    : speed_x{1}, speed_y{0}
{
  sprite = std::make_unique<Sprite>(renderer, "../assets/pacman.png", 4, 16);
  sprite->SetFrames({1, 2});
}

void Pacman::Update(const float deltaTime)
{
    x += speed_x;
    if (x > 150) {
        x = 0;
    }
    y += speed_y;
    if (y > 150) {
        y = 0;
    }

    sprite->Update(deltaTime);
}

void Pacman::Render(SDL_Renderer *renderer)
{
    sprite->Render(renderer, x, y);
}

void Pacman::ProcessInput(const Uint8 *state)
{
    if (state[SDL_SCANCODE_RIGHT]) {
        sprite->SetFrames({1, 2});
        speed_x = 1;
        speed_y = 0;
    } else if (state[SDL_SCANCODE_LEFT]) {
        sprite->SetFrames({3, 4});
        speed_x = -1;
        speed_y = 0;
    } else if (state[SDL_SCANCODE_UP]) {
        sprite->SetFrames({5, 6});
        speed_x = 0;
        speed_y = -1;
    } else if (state[SDL_SCANCODE_DOWN]) {
        sprite->SetFrames({7, 8});
        speed_x = 0;
        speed_y = 1;
    }
}
