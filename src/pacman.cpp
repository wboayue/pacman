#include <cmath>
#include <iostream>

#include "pacman.h"

static constexpr Vec2 kHomePosition{14 * 8 + 4, 26 * 8 + 4};

Pacman::Pacman(SDL_Renderer *renderer)
    : position{kHomePosition}, velocity{0, 0}, heading{Direction::kNeutral} {
  sprite = std::make_unique<Sprite>(renderer, "../assets/pacman.png", 8, 16);
  sprite->SetFrames({1, 2});
}

auto Pacman::Update(const float deltaTime, Grid &grid, GameState &state, AudioSystem &audio)
    -> void {
  // Check that I can turn in requested direction
  if (grid.GetCell(NextGridPosition(heading)) != Cell::kWall) {
    // setSpriteForHeading
    if (heading == Direction::kEast) {
      sprite->SetFrames({1, 2});
      velocity = Vec2{kMaxSpeed * 0.8f, 0};
    } else if (heading == Direction::kWest) {
      sprite->SetFrames({3, 4});
      velocity = Vec2{-kMaxSpeed * 0.8f, 0};
    } else if (heading == Direction::kNorth) {
      sprite->SetFrames({5, 6});
      velocity = Vec2{0, -kMaxSpeed * 0.8f};
    } else if (heading == Direction::kSouth) {
      sprite->SetFrames({7, 8});
      velocity = Vec2{0, kMaxSpeed * 0.8f};
    } else {
      sprite->SetFrames({1, 2});
    }
  }

  auto nextPosition = NextGridPosition();
  if (grid.GetCell(nextPosition) == Cell::kWall) {
    // handle collision with wall
    float x = position.x / 8.0;
    float y = position.y / 8.0;
    if (velocity.x > 0) {
      if ((x - floor(x)) > 0.5) {
        velocity.x = 0;
        position.x = floor(x) * 8 + 4;
      }
    } else if (velocity.x < 0) {
      if (!((x - floor(x)) < 0.5)) {
        velocity.x = 0;
        position.x = floor(x) * 8 + 4;
      }
    } else if (velocity.y > 0) {
      if ((y - floor(y)) > 0.5) {
        velocity.y = 0;
        position.y = floor(y) * 8 + 4;
      }
    } else if (velocity.y < 0) {
      if ((y - floor(y)) < 0.5) {
        velocity.y = 0;
        position.y = floor(y) * 8 + 4;
      }
    }
  }

  if (grid.HasPellet(GetGridPosition())) {
    // handle energizer
    auto pellet = grid.ConsumePellet(GetGridPosition());
    if (pellet->IsEnergizer()) {
      state.score += 50;
      state.mode = GhostMode::kScared;
      audio.PlayAsync(Sound::kPowerPellet, 5);
    } else {
      state.score += 10;
      audio.PlayAsync(Sound::kMunch1);
    }
    state.pelletsConsumed += 1;
    if (state.pelletsConsumed == 244) {
      state.levelCompleted = true;
    }
    // std::cout << "pellets  " << state.pelletsConsumed << std::endl;
  } else {
    // update position
    position += (velocity * deltaTime);

    if (velocity.x > 0 || velocity.x < 0) {
      position.y = floor(((int)position.y / 8) * 8 + 4);
    }
    if (velocity.y > 0 || velocity.y < 0) {
      position.x = floor(((int)position.x / 8) * 8 + 4);
    }
  }

  // teleport on side
  if (position.x < -16) {
    position.x = kGridWidth * 8 + 16;
  } else if (position.x > kGridWidth * 8 + 16) {
    position.x = -16;
  }

  sprite->Update(deltaTime);
}

auto Pacman::Reset() -> void {
  velocity = Vec2{0, 0};
  position = Vec2{14 * 8 + 4, 26 * 8 + 4};
  heading = Direction::kNeutral;
}

auto Pacman::Render(SDL_Renderer *renderer) -> void {
  sprite->Render(renderer, {floor(position.x - 8), floor(position.y - 8)});
}

auto Pacman::ProcessInput(const Uint8 *state) -> void {
  if (state[SDL_SCANCODE_RIGHT]) {
    heading = Direction::kEast;
  } else if (state[SDL_SCANCODE_LEFT]) {
    heading = Direction::kWest;
  } else if (state[SDL_SCANCODE_UP]) {
    heading = Direction::kNorth;
  } else if (state[SDL_SCANCODE_DOWN]) {
    heading = Direction::kSouth;
  }
}

auto Pacman::GetPosition() const -> Vec2 { return position; }

auto Pacman::GetHeading() const -> Direction { return heading; }

auto Pacman::GetGridPosition() const -> Vec2 {
  auto t = position / 8;
  return {floor(t.x), floor(t.y)};
}

auto Pacman::NextGridPosition() const -> Vec2 {
  auto currentPosition = GetGridPosition();

  if (velocity.x > 0) {
    return {currentPosition.x + 1, currentPosition.y};
  }
  if (velocity.x < 0) {
    return {currentPosition.x - 1, currentPosition.y};
  }
  if (velocity.y > 0) {
    return {currentPosition.x, currentPosition.y + 1};
  }
  if (velocity.y < 0) {
    return {currentPosition.x, currentPosition.y - 1};
  }

  return currentPosition;
}

auto Pacman::NextGridPosition(const Direction &direction) const -> Vec2 {
  auto currentPosition = GetGridPosition();

  switch (direction) {
  case Direction::kEast:
    return {currentPosition.x + 1, currentPosition.y};

  case Direction::kWest:
    return {currentPosition.x - 1, currentPosition.y};

  case Direction::kNorth:
    return {currentPosition.x, currentPosition.y - 1};

  case Direction::kSouth:
    return {currentPosition.x, currentPosition.y + 1};

  default:
    return currentPosition;
  }
}
