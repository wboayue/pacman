#include <cmath>
#include <iostream>

#include "constants.h"
#include "pacman.h"

static constexpr Vec2 kHomePosition{13 * kCellSize + (kCellSize/2.0), 26 * kCellSize + (kCellSize/2.0)};

auto framesForHeading(const Direction &direction) -> std::vector<int>;
auto velocityForHeading(const Direction &direction) -> Vec2;
auto headingForVelocity(const Vec2& velocity) -> Direction;
auto center(float pos) -> float;
auto boundUpper(float pos) -> float;
auto boundLower(float pos) -> float;


Pacman::Pacman(SDL_Renderer *renderer)
    : position_{kHomePosition}, velocity_{0, 0}, heading_{Direction::kNeutral} {
  sprite_ = std::make_unique<Sprite>(renderer, "../assets/pacman.png", 8, 16);
  sprite_->SetFrames({1, 2});
}

auto Pacman::Update(const float deltaTime, Grid& grid, GameState& state, AudioSystem& audio, std::vector<std::shared_ptr<Ghost>>& ghosts)
    -> void {

  if (!isInTunnel()) {
    // Updates velocity based on input if not in tunnel.
    auto requestedPosition = NextCell(heading_);
    if (grid.GetCell(requestedPosition) != Cell::kWall) {
      sprite_->SetFrames(framesForHeading(heading_));
      velocity_ = velocityForHeading(heading_);
    }
  }

  updatePosition(deltaTime);

  auto currentHeading = headingForVelocity(velocity_);
  auto nextPosition = NextCell(currentHeading);
  if (grid.GetCell(nextPosition) == Cell::kWall) {
    switch (currentHeading) {
      case Direction::kEast:
        position_.x = boundUpper(position_.x);
        break;
      case Direction::kWest:
        position_.x = boundLower(position_.x);
        break;
      case Direction::kNorth:
        position_.y = boundLower(position_.y);
        break;
      case Direction::kSouth:
        position_.y = boundUpper(position_.y);
        break;
      default:
        break;
    }
  }

  energizedFor_ -= deltaTime;
  if (energizedFor_ < 0.0) {
    energizedFor_ = 0.0;
  }

  if (IsEnergized()) {
    auto currentPosition = GetCell();
    for (auto &ghost : ghosts) {
      if (currentPosition == ghost->GetCell() && !ghost->IsReSpawning()) {
        state.score += kEnergizerPoints;
        audio.PlayAsync(Sound::kPowerPellet, 5);
        ghost->ReSpawn();
      }
    }
  }

  if (grid.HasPellet(GetCell())) {
    // handle energizer
    auto pellet = grid.ConsumePellet(GetCell());
    if (pellet->IsEnergizer()) {
      state.score += kEnergizerPoints;
      energizedFor_ = 6.0;
//      state.mode = GhostMode::kScared;
      audio.PlayAsync(Sound::kPowerPellet, 5);
    } else {
      state.score += kPelletPoints;
      audio.PlayAsync(Sound::kMunch1);
    }

    state.pelletsConsumed += 1;
    state.levelCompleted = state.pelletsConsumed == kTotalPellets;
  } 

  sprite_->Update(deltaTime);
}

auto Pacman::isInTunnel() -> bool {
  auto currentPosition = GetCell();

  if (currentPosition.y != kTunnelRow) {
    return false;
  }

  return currentPosition.x < 1 || currentPosition.x > 26;
}

/**
 * Updates the position of Pacman given time change
 * 
 * @param timeDelta 
 */
auto Pacman::updatePosition(float timeDelta) -> void {
  position_ += (velocity_ * timeDelta);

  //  teleport on side
  if (position_.x < -16) {
    position_.x = kGridWidth * kCellSize + 16;
  } else if (position_.x > kGridWidth * kCellSize + 16) {
    position_.x = -16;
  }

  // Ensures Pacman is centered in Grid
  switch (headingForVelocity(velocity_)) {
    case Direction::kEast:
    case Direction::kWest:
      position_.y = center(position_.y);
      break;
    case Direction::kNorth:
    case Direction::kSouth:
      position_.x = center(position_.x);
      break;
    default:
      break;
  }
}

auto Pacman::Reset() -> void {
  velocity_ = Vec2{0, 0};
  position_ = kHomePosition;
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

/**
 * GetHeading return Pacman's current heading
 * @return current heading (Direction) 
 */
auto Pacman::GetHeading() const -> Direction { return heading_; }

/**
 * GetCell returns the current cell occupied by Pacman.
 * @return current cell (Vec2) 
 */
auto Pacman::GetCell() const -> Vec2 {
  return (position_ / kCellSize).Floor();
}

/**
 * NextCell determines next cell that Pacman will occupy
 * based on `direction` and current cell.
 * @param direction cardinal heading of Pacman.
 * @return next cell based on heading (Vec2)
 */
auto Pacman::NextCell(const Direction &direction) const -> Vec2 {
  auto currentCell = GetCell();

  switch (direction) {
  case Direction::kEast:
    return {currentCell.x + 1, currentCell.y};

  case Direction::kWest:
    return {currentCell.x - 1, currentCell.y};

  case Direction::kNorth:
    return {currentCell.x, currentCell.y - 1};

  case Direction::kSouth:
    return {currentCell.x, currentCell.y + 1};

  default:
    return currentCell;
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

auto center(float pos) -> float {
  return floor(((int)pos / kCellSize) * kCellSize + (kCellSize/2));
}

auto boundUpper(float pos) -> float {
  auto max = center(pos);
  return pos > max ? max : pos;
}

auto boundLower(float pos) -> float {
  auto min = center(pos);
  return pos < min ? min : pos;
}
