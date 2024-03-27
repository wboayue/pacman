#ifndef PACMAN_H
#define PACMAN_H

#include <string>
#include <vector>

#include "SDL.h"

#include "audio-system.h"
#include "constants.h"
#include "game-state.h"
#include "grid.h"
#include "ghost.h"
#include "sprite.h"
#include "vector2.h"

class Ghost;

class Pacman {
public:
  Pacman(SDL_Renderer *renderer);

  auto Update(const float deltaTime, Grid& grid, GameState& state, AudioSystem& audio, std::vector<std::shared_ptr<Ghost>>& ghosts
) -> void;
  auto Render(SDL_Renderer *renderer) -> void;
  auto ProcessInput(const Uint8 *state) -> void;

  auto GetPosition() const -> Vec2;
  auto GetHeading() const -> Direction;
  auto GetGridPosition() const -> Vec2;
  auto NextGridPosition(const Direction &direction) const -> Vec2;
  auto Reset() -> void;
  auto IsEnergized() -> bool const { return energizedFor_ > 0.0; };

private:

  auto updatePosition(float timeDelta) -> void;
  auto isInTunnel() -> bool;

  Vec2 position_;
  Vec2 velocity_;
  Direction heading_;
  float energizedFor_ = 0.0;

  std::unique_ptr<Sprite> sprite_;
};

#endif