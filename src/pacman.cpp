#include <iostream>
#include <math.h>

#include "pacman.h"

Pacman::Pacman(SDL_Renderer *renderer)
    : velocity{0, 0}, position{14 * 8, 26 * 8 + 4}, heading{
                                                        Direction::kNeutral} {
  sprite = std::make_unique<Sprite>(renderer, "../assets/pacman.png", 4, 16);
  sprite->SetFrames({1, 2});
}

void Pacman::Update(const float deltaTime, Grid &grid, GameState &state) {
  // Check that I can turn in requested direction
  if (grid.GetCell(NextGridPosition(heading)) != Cell::kWall) {
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
    }
  }

  auto nextPosition = NextGridPosition();
  if (grid.GetCell(nextPosition) == Cell::kWall) {
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
    auto pellet = grid.ConsumePellet(GetGridPosition());
    if (pellet->IsEnergizer()) {
      state.score += 50;
    } else {
      state.score += 10;
    }
    //        std::cout << "score:  " << state.score << std::endl;
  } else {
    position += (velocity * deltaTime);

    if (velocity.x > 0 || velocity.x < 0) {
      position.y = floor(((int)position.y / 8) * 8 + 4);
    }
    if (velocity.y > 0 || velocity.y < 0) {
      position.x = floor(((int)position.x / 8) * 8 + 4);
    }
  }

  // teleport
  if (position.x < -16) {
    position.x = kGridWidth * 8 + 16;
  } else if (position.x > kGridWidth * 8 + 16) {
    position.x = -16;
  }

  sprite->Update(deltaTime);
}

void Pacman::Render(SDL_Renderer *renderer) {
  sprite->Render(renderer, {floor(position.x - 8), floor(position.y - 8)});
}

void Pacman::ProcessInput(const Uint8 *state) {
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

Vec2 Pacman::GetPosition() { return position; }

Vec2 Pacman::GetGridPosition() {
  auto t = position / 8;
  return {floor(t.x), floor(t.y)};
}

Vec2 Pacman::NextGridPosition() {
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

Vec2 Pacman::NextGridPosition(const Direction &direction) {
  auto currentPosition = GetGridPosition();

  switch (direction) {
  case kEast:
    return {currentPosition.x + 1, currentPosition.y};

  case kWest:
    return {currentPosition.x - 1, currentPosition.y};

  case kNorth:
    return {currentPosition.x, currentPosition.y - 1};

  case kSouth:
    return {currentPosition.x, currentPosition.y + 1};

  default:
    return currentPosition;
  }
}
