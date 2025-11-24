#include "pellet.h"

Pellet::Pellet(SDL_Renderer *renderer, const Vec2 position) : Pellet(renderer, position, false) {}

Pellet::Pellet(SDL_Renderer *renderer, const Vec2 position, bool power) : position{position}, power{power} {
  if (power) {
    sprite = std::make_unique<Sprite>(renderer, Sprites::kPowerPellet, 3, 8);
    sprite->SetFrames({1, 2});
  } else {
    sprite = std::make_unique<Sprite>(renderer, Sprites::kPellet);
  }
}

auto Pellet::Update(const float deltaTime) -> void { sprite->Update(deltaTime); }

auto Pellet::Render(SDL_Renderer *renderer) -> void { sprite->Render(renderer, {position.x * 8, position.y * 8}); }

auto Pellet::IsEnergizer() const -> bool { return power; }
