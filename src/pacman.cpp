#include <cmath>
#include <iostream>

#include "constants.h"
#include "pacman.h"

static constexpr Vec2 kHomePosition{14 * kCellSize + 4, 26 * kCellSize + 4};

auto framesForHeading(const Direction &direction) -> std::vector<int>;
auto velocityForHeading(const Direction &direction) -> Vec2;
auto headingForVelocity(const Vec2& velocity) -> Direction;


Pacman::Pacman(SDL_Renderer *renderer)
    : position_{kHomePosition}, velocity_{0, 0}, heading_{Direction::kNeutral} {
  sprite_ = std::make_unique<Sprite>(renderer, "../assets/pacman.png", 8, 16);
  sprite_->SetFrames({1, 2});
}

auto Pacman::Update(const float deltaTime, Grid &grid, GameState &state, AudioSystem &audio)
    -> void {

  // Updates velocity based on user input.
  auto requestedPosition = NextGridPosition(heading_);
  if (grid.GetCell(requestedPosition) != Cell::kWall) {
    sprite_->SetFrames(framesForHeading(heading_));
    velocity_ = velocityForHeading(heading_);
  }

  auto nextPosition = NextGridPosition(headingForVelocity(velocity_));
  if (grid.GetCell(nextPosition) == Cell::kWall) {
    // use heading
    // handle collision with wall
    float x = position_.x / 8.0;
    float y = position_.y / 8.0;
    if (velocity_.x > 0) {
      if ((x - floor(x)) > 0.5) {
        velocity_.x = 0;
        position_.x = floor(x) * 8 + 4;
      }
    } else if (velocity_.x < 0) {
      if (!((x - floor(x)) < 0.5)) {
        velocity_.x = 0;
        position_.x = floor(x) * 8 + 4;
      }
    } else if (velocity_.y > 0) {
      if ((y - floor(y)) > 0.5) {
        velocity_.y = 0;
        position_.y = floor(y) * 8 + 4;
      }
    } else if (velocity_.y < 0) {
      if ((y - floor(y)) < 0.5) {
        velocity_.y = 0;
        position_.y = floor(y) * 8 + 4;
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
    position_ += (velocity_ * deltaTime);

    if (velocity_.x > 0 || velocity_.x < 0) {
      position_.y = floor(((int)position_.y / 8) * 8 + 4);
    }
    if (velocity_.y > 0 || velocity_.y < 0) {
      position_.x = floor(((int)position_.x / 8) * 8 + 4);
    }
  }

  // teleport on side
  if (position_.x < -16) {
    position_.x = kGridWidth * kCellSize + 16;
  } else if (position_.x > kGridWidth * kCellSize + 16) {
    position_.x = -16;
  }

  sprite_->Update(deltaTime);
}

auto Pacman::Reset() -> void {
  velocity_ = Vec2{0, 0};
  position_ = Vec2{14 * kCellSize + 4, 26 * kCellSize + 4};
  heading_ = Direction::kNeutral;
}

auto Pacman::Render(SDL_Renderer *renderer) -> void {
  sprite_->Render(renderer, {floor(position_.x - kCellSize), floor(position_.y - kCellSize)});
}

auto Pacman::ProcessInput(const Uint8 *state) -> void {
  if (state[SDL_SCANCODE_RIGHT]) {
    heading_ = Direction::kEast;
  } else if (state[SDL_SCANCODE_LEFT]) {
    heading_ = Direction::kWest;
  } else if (state[SDL_SCANCODE_UP]) {
    heading_ = Direction::kNorth;
  } else if (state[SDL_SCANCODE_DOWN]) {
    heading_ = Direction::kSouth;
  }
}

auto Pacman::GetPosition() const -> Vec2 { return position_; }

auto Pacman::GetHeading() const -> Direction { return heading_; }

auto Pacman::GetGridPosition() const -> Vec2 {
  auto t = position_ / kCellSize;
  return {floor(t.x), floor(t.y)};
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

/**
 * Determines the sequence of frames to animate for movement in given heading.
 * @param direction direction pacman is moving
 * @return sequence of frames to animate
 */
auto framesForHeading(const Direction &direction) -> std::vector<int> {
  switch (direction) {
    case Direction::kEast:
      return {1, 2};
    case Direction::kWest:
      return {3, 4};
    case Direction::kNorth:
      return {5, 6};
    case Direction::kSouth:
      return {7, 8};
    case Direction::kNeutral:
      return {1, 2};
  }
}

/**
 * Computes the velocity Vector for the given heading
 * @param direction direction pacman is moving
 * @return velocity vector
 */
auto velocityForHeading(const Direction &direction) -> Vec2 {
  switch (direction) {
    case Direction::kEast:
      return Vec2{kMaxSpeed * 0.8f, 0};
    case Direction::kWest:
      return Vec2{-kMaxSpeed * 0.8f, 0};
    case Direction::kNorth:
      return Vec2{0, -kMaxSpeed * 0.8f};
    case Direction::kSouth:
      return Vec2{0, kMaxSpeed * 0.8f};
    case Direction::kNeutral:
      return Vec2{kMaxSpeed * 0.8f, 0};
  }
}

auto headingForVelocity(const Vec2& velocity) -> Direction {
  if (velocity.x > 0) {
    return Direction::kEast;
  } else if (velocity.x < 0) {
    return Direction::kWest;
  } else if (velocity.y > 0) {
    return Direction::kSouth;
  } else if (velocity.y < 0) {
    return Direction::kNorth;
  } else {
    return Direction::kNeutral;
  }
}
