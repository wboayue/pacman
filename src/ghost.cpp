#include <iostream>

#include "constants.h"
#include "ghost.h"

static constexpr std::array<Candidate, 4> options{
    Candidate{{0, -1}, Direction::kNorth},
    Candidate{{0, 1}, Direction::kSouth},
    Candidate{{1, 0}, Direction::kEast},
    Candidate{{-1, 0}, Direction::kWest},
};

auto center(float pos) -> float;
auto boundUpper(float pos) -> float;
auto boundLower(float pos) -> float;

auto reverseDirection(Direction direction) -> Direction {
  switch (direction) {
  case Direction::kNorth:
    return Direction::kSouth;
  case Direction::kSouth:
    return Direction::kNorth;
  case Direction::kEast:
    return Direction::kWest;
  case Direction::kWest:
    return Direction::kEast;
  default:
    return Direction::kNeutral;
  }
}

auto toCell(const Vec2 &position) -> Vec2 {
  auto t = position / kCellSize;
  return {floor(t.x), floor(t.y)};
}

Ghost::Ghost(const GhostConfig &config)
    : initialPosition_{config.GetInitialPosition()}, heading_{config.GetInitialHeading()},
      targeter{config.GetTargeter()}, scatterCell{config.GetScatterCell()}, sprite{config.GetSprite()},
      scaredSprite{config.GetScaredSprite()}, reSpawnSprite{config.GetReSpawnSprite()} {
  position_ = initialPosition_;
  setFramesForHeading(heading_);
}

auto Ghost::Update(const float deltaTime, Grid &grid, GameContext &context, Pacman &pacman, Ghost &blinky) -> void {
  if (isInPen()) {
    // update penned
    if (active_) {
      exitPen(deltaTime);
    } else {
      penDance(deltaTime);
    }
  } else if (isInTunnel()) {
    position_ += (velocity_ * deltaTime);

    // teleport on side
    if (position_.x < -16) {
      position_.x = kGridWidth * kCellSize + 16;
    } else if (position_.x > kGridWidth * kCellSize + 16) {
      position_.x = -16;
    }
  } else {
    // update active

    if (mode_ != GhostMode::kReSpawn) {
      if (pacman.IsEnergized()) {
        mode_ = GhostMode::kScared;
      } else {
        mode_ = GhostMode::kChase;
      }
    }

    if (inCellCenter()) {
      auto candidates_ = candidates(grid);
      if (candidates_.size() == 1) {
        heading_ = candidates_[0].heading;
      } else {
        auto target = targeter(*this, pacman, blinky, mode_);
        if (mode_ == GhostMode::kReSpawn) {
          target = toCell(initialPosition_);
        }
        moveTowards(grid, target);
      }

      setFramesForHeading(heading_);
      setVelocityForHeading(heading_);
    }

    if (previousCell_ != GetCell()) {
      previousHeading_ = heading_;
    }

    previousCell_ = GetCell();

    if (mode_ == GhostMode::kReSpawn && previousCell_ == toCell(initialPosition_)) {
      mode_ = GhostMode::kChase;
    }

    position_ += (velocity_ * deltaTime);

    auto nextPosition = nextCell(heading_);
    if (grid.GetCell(nextPosition) == Cell::kWall) {
      switch (heading_) {
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
  }

  sprite->Update(deltaTime);
  scaredSprite->Update(deltaTime);
  reSpawnSprite->Update(deltaTime);
}

auto Ghost::Pause() -> void {}

auto Ghost::Resume() -> void {}

auto Ghost::nextCell(const Direction &direction) const -> Vec2 {
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

auto Ghost::isInTunnel() -> bool {
  auto currentCell = GetCell();

  if (currentCell.y != kTunnelRow) {
    return false;
  }

  return currentCell.x < 4 || currentCell.x > 22;
}

auto Ghost::moveTowards(Grid &grid, const Vec2 &target) -> Direction {
  auto candidates_ = candidates(grid);

  if (candidates_.empty()) {
    heading_ = reverseDirection(heading_);
  } else {
    auto closest = candidates_.at(0);
    for (auto &candidate : candidates_) {
      if (candidate.position.Distance(target) < closest.position.Distance(target)) {
        closest = candidate;
      }
    }
    heading_ = closest.heading;
  }

  if (heading_ == Direction::kEast || heading_ == Direction::kWest) {
    position_.y = floor(((int)position_.y / kCellSize) * kCellSize + (kCellSize / 2));
  }
  if (heading_ == Direction::kNorth || heading_ == Direction::kSouth) {
    position_.x = floor(((int)position_.x / kCellSize) * kCellSize + (kCellSize / 2));
  }
}

auto Ghost::inCellCenter() -> bool {
  float centerX = center(position_.x);
  float centerY = center(position_.y);

  return (heading_ == Direction::kNorth && position_.y <= centerY) ||
         (heading_ == Direction::kSouth && position_.y >= centerY) ||
         (heading_ == Direction::kEast && position_.x >= centerX) ||
         (heading_ == Direction::kWest && position_.x <= centerX);
}

auto Ghost::atDecisionPoint(Grid &grid) const -> bool {
  static constexpr auto kMidPoint = 0.5f;

  float x = position_.x - floor(position_.x);
  float y = position_.y - floor(position_.y);

  // reach middle of cell?
  if ((heading_ == Direction::kNorth && y > kMidPoint) || (heading_ == Direction::kSouth && y < kMidPoint) ||
      (heading_ == Direction::kEast && x < kMidPoint) || (heading_ == Direction::kWest && x > kMidPoint)) {
    return false;
  }

  // will i run into a wall?
  if (grid.GetCell(nextCell(heading_)) == Cell::kWall) {
    return true;
  }

  switch (heading_) {
  case Direction::kNorth:
  case Direction::kSouth:
    return grid.GetCell(nextCell(Direction::kEast)) != Cell::kWall ||
           grid.GetCell(nextCell(Direction::kWest)) != Cell::kWall;
  case Direction::kEast:
  case Direction::kWest:
    return grid.GetCell(nextCell(Direction::kNorth)) == Cell::kWall ||
           grid.GetCell(nextCell(Direction::kSouth)) == Cell::kWall;
  default:
    return true;
  }
}

auto Ghost::candidates(Grid &grid) -> std::vector<Candidate> {

  auto results = std::vector<Candidate>{};
  for (auto option : options) {
    if (reverseDirection(option.heading) == previousHeading_) {
      continue;
    }

    auto pos = GetCell() + option.position;
    if (grid.GetCell(pos) == Cell::kWall) {
      continue;
    }

    results.push_back(Candidate{pos, option.heading});
  }

  return results;
}

auto Ghost::Render(SDL_Renderer *renderer) -> void {
  if (mode_ == GhostMode::kScared) {
    scaredSprite->Render(renderer, {floor(position_.x - kCellSize), floor(position_.y - kCellSize)});
  } else if (mode_ == GhostMode::kReSpawn) {
    reSpawnSprite->Render(renderer, {floor(position_.x - kCellSize), floor(position_.y - kCellSize)});
  } else {
    sprite->Render(renderer, {floor(position_.x - kCellSize), floor(position_.y - kCellSize)});
  }
}

auto Ghost::Reset() -> void {
  position_ = initialPosition_;
  mode_ = GhostMode::kChase;
}

auto Ghost::ReSpawn() -> void { mode_ = GhostMode::kReSpawn; }

auto Ghost::setFramesForHeading(Direction heading) -> void {
  switch (heading) {
  case Direction::kNorth:
    sprite->SetFrames({4, 5});
    reSpawnSprite->SetFrames({2});
    break;

  case Direction::kSouth:
    sprite->SetFrames({6, 7});
    reSpawnSprite->SetFrames({3});
    break;

  case Direction::kEast:
    sprite->SetFrames({0, 1});
    reSpawnSprite->SetFrames({1});
    break;

  case Direction::kWest:
    sprite->SetFrames({2, 3});
    reSpawnSprite->SetFrames({1});
    break;

  default:
    sprite->SetFrames({2, 3});
    break;
  }
}

// velocity for heading
auto Ghost::setVelocityForHeading(Direction heading) -> void {
  switch (heading) {
  case Direction::kNorth:
    velocity_ = Vec2{0, kMaxSpeed * -kGhostSpeedMultiplier};
    break;

  case Direction::kSouth:
    velocity_ = Vec2{0, kMaxSpeed * kGhostSpeedMultiplier};
    break;

  case Direction::kEast:
    velocity_ = Vec2{kMaxSpeed * kGhostSpeedMultiplier, 0};
    break;

  case Direction::kWest:
    velocity_ = Vec2{kMaxSpeed * -kGhostSpeedMultiplier, 0};
    break;

  default:
    velocity_ = Vec2{0, 0};
    break;
  }
}

auto Ghost::isInPen() -> bool {
  auto pos = GetCell();
  return pos.x >= 12 && pos.x <= 16 && pos.y >= 17 && pos.y <= 18;
}

auto Ghost::GetCell() const -> Vec2 {
  auto t = position_ / kCellSize;
  return {floor(t.x), floor(t.y)};
}

auto Ghost::exitPen(const float deltaTime) -> void {
  heading_ = Direction::kNorth;

  setFramesForHeading(heading_);
  setVelocityForHeading(heading_);

  position_ += (velocity_ / 2.0 * deltaTime);
}

auto Ghost::penDance(const float deltaTime) -> void {
  setFramesForHeading(heading_);
  setVelocityForHeading(heading_);

  position_ += (velocity_ / 2.0 * deltaTime); // NOLINT(cppcoreguidlines-avoid-magic-numbers)

  if (position_.y < kPenTop) {
    position_.y = kPenTop;
    heading_ = reverseDirection(heading_);
  }

  if (position_.y > kPenBottom) {
    position_.y = kPenBottom;
    heading_ = reverseDirection(heading_);
  }

  setFramesForHeading(heading_);
  setVelocityForHeading(heading_);
}

// Blinky

BlinkyConfig::BlinkyConfig(SDL_Renderer *renderer) : GhostConfig{renderer} {}

auto BlinkyConfig::GetTargeter() const -> Targeter {
  return [](Ghost &me, Pacman &pacman, [[maybe_unused]] Ghost &blinky, GhostMode mode) {
    if (mode == GhostMode::kScatter || mode == GhostMode::kScared) {
      return me.GetScatterCell();
    }

    return pacman.GetCell();
  };
}

auto BlinkyConfig::GetScatterCell() const -> Vec2 { return kBlinkyScatterCell; }

auto BlinkyConfig::GetSprite() const -> std::unique_ptr<Sprite> {
  return std::make_unique<Sprite>(renderer, "../assets/sprites/blinky.png", kGhostFps, kGhostFrameWidth);
}

auto BlinkyConfig::GetInitialPosition() const -> Vec2 {
  return Vec2{kBlinkyStartCell.x * kCellSize, kBlinkyStartCell.y * kCellSize + (kCellSize / 2)};
}

auto BlinkyConfig::GetInitialHeading() const -> Direction { return Direction::kWest; }

// Inky

InkyConfig::InkyConfig(SDL_Renderer *renderer) : GhostConfig{renderer} {}

auto InkyConfig::GetTargeter() const -> Targeter {
  return [](Ghost &me, Pacman &pacman, Ghost &blinky, GhostMode mode) {
    if (mode == GhostMode::kScatter || mode == GhostMode::kScared) {
      return me.GetScatterCell();
    }
    Vec2 target = pacman.GetCell();

    if (pacman.GetHeading() == Direction::kNorth) {
      target += Vec2{0, -kInkyPacmanOffset};
    } else if (pacman.GetHeading() == Direction::kSouth) {
      target += Vec2{0, kInkyPacmanOffset};
    } else if (pacman.GetHeading() == Direction::kEast) {
      target += Vec2{kInkyPacmanOffset, 0};
    } else if (pacman.GetHeading() == Direction::kWest) {
      target += Vec2{-kInkyPacmanOffset, 0};
    }

    auto projection = target - blinky.GetCell();
    projection = projection * 2;

    return blinky.GetCell() + projection;
  };
};

auto InkyConfig::GetScatterCell() const -> Vec2 { return kInkyScatterCell; }

auto InkyConfig::GetSprite() const -> std::unique_ptr<Sprite> {
  return std::make_unique<Sprite>(renderer, "../assets/sprites/inky.png", kGhostFps, kGhostFrameWidth);
}

auto InkyConfig::GetInitialPosition() const -> Vec2 {
  return Vec2{kInkyStartCell.x * kCellSize, kInkyStartCell.y * kCellSize + (kCellSize / 2)};
}

auto InkyConfig::GetInitialHeading() const -> Direction { return Direction::kNorth; }

// Pinky

PinkyConfig::PinkyConfig(SDL_Renderer *renderer) : GhostConfig{renderer} {}

auto PinkyConfig::GetTargeter() const -> Targeter {
  return [](Ghost &me, Pacman &pacman, [[maybe_unused]] Ghost &blinky, GhostMode mode) {
    if (mode == GhostMode::kScatter || mode == GhostMode::kScared) {
      return me.GetScatterCell();
    }

    Vec2 target = pacman.GetCell();

    if (pacman.GetHeading() == Direction::kNorth) {
      target += Vec2{0, -kPinkyTargetOffset};
    } else if (pacman.GetHeading() == Direction::kSouth) {
      target += Vec2{0, kPinkyTargetOffset};
    } else if (pacman.GetHeading() == Direction::kEast) {
      target += Vec2{kPinkyTargetOffset, 0};
    } else if (pacman.GetHeading() == Direction::kWest) {
      target += Vec2{-kPinkyTargetOffset, 0};
    }

    return target;
  };
}

auto PinkyConfig::GetScatterCell() const -> Vec2 { return kPinkyScatterCell; }

auto PinkyConfig::GetSprite() const -> std::unique_ptr<Sprite> {
  return std::make_unique<Sprite>(renderer, "../assets/sprites/pinky.png", kGhostFps, kGhostFrameWidth);
}

auto PinkyConfig::GetInitialPosition() const -> Vec2 {
  return Vec2{kPinkyStartCell.x * kCellSize, kPinkyStartCell.y * kCellSize + (kCellSize / 2)};
}

auto PinkyConfig::GetInitialHeading() const -> Direction { return Direction::kSouth; }

// Clyde

ClydeConfig::ClydeConfig(SDL_Renderer *renderer) : GhostConfig{renderer} {}

auto ClydeConfig::GetTargeter() const -> Targeter {
  return [](Ghost &me, Pacman &pacman, [[maybe_unused]] Ghost &blinky, GhostMode mode) {
    if (mode == GhostMode::kScatter || mode == GhostMode::kScared) {
      return me.GetScatterCell();
    }

    auto d = me.GetCell().Distance(pacman.GetCell());
    if (d > kClydeRelaxDistance) {
      return pacman.GetCell();
    }

    return me.GetScatterCell();
  };
}

auto ClydeConfig::GetScatterCell() const -> Vec2 { return kClydeScatterCell; }

auto ClydeConfig::GetSprite() const -> std::unique_ptr<Sprite> {
  return std::make_unique<Sprite>(renderer, "../assets/sprites/clyde.png", kGhostFps, kGhostFrameWidth);
}

auto ClydeConfig::GetInitialPosition() const -> Vec2 {
  return Vec2{kClydeStartCell.x * kCellSize, kClydeStartCell.y * kCellSize + (kCellSize / 2)};
}

auto ClydeConfig::GetInitialHeading() const -> Direction { return Direction::kNorth; }

GhostConfig::GhostConfig(SDL_Renderer *renderer) : renderer{renderer} {}

auto GhostConfig::GetScaredSprite() const -> std::unique_ptr<Sprite> {
  return std::make_unique<Sprite>(renderer, "../assets/sprites/scared-ghost.png", kGhostFps, kGhostFrameWidth);
}

auto GhostConfig::GetReSpawnSprite() const -> std::unique_ptr<Sprite> {
  return std::make_unique<Sprite>(renderer, "../assets/sprites/ghost-eyes.png", kGhostFps, kGhostFrameWidth);
}

// defines state machines for ghost behavior

enum class GhostStates {
  kPenned,
  kExitPen,
  kChase,
  kScatter,
  kScared,
  kReSpawn,
};

// Base state interface
class GhostState {
public:
  virtual ~GhostState() = default;

  // Core state methods
  virtual auto Enter(GameContext &context, Ghost &ghost) -> void {};
  virtual auto Tick(GameContext &context, Ghost &ghost, float deltaTime) -> GhostStates = 0;
};

struct PennedState : GhostState {
  auto Enter(GameContext &context, Ghost &ghost) -> void override { ghost.Reset(); }

  auto Tick(GameContext &context, Ghost &ghost, float deltaTime) -> GhostStates override {
    if (ghost.IsChasing()) {
      return GhostStates::kExitPen;
    }

    return GhostStates::kPenned;
  }
};

struct ExitPenState : GhostState {
  auto Enter(GameContext &context, Ghost &ghost) -> void override { ghost.Reset(); }

  auto Tick(GameContext &context, Ghost &ghost, float deltaTime) -> GhostStates override {
    if (ghost.GetCell().y < kPenTop) {
      return GhostStates::kChase;
    }

    return GhostStates::kExitPen;
  }
};

struct ChaseState : GhostState {
  auto Tick(GameContext &context, Ghost &ghost, float deltaTime) -> GhostStates override {
    if (context.paused) {
      return GhostStates::kChase;
    }

    // ghost.Update(deltaTime, context.grid, context, *context.pacman, *context.ghosts[0]);

    if (ghost.IsReSpawning()) {
      return GhostStates::kReSpawn;
    }

    return GhostStates::kChase;
  }
};

// blinky, pinky, inky, clyde

struct ScatterState : GhostState {

  ScatterState(const Vec2 &scatterTarget) : scatterTarget{scatterTarget} {};

  auto Tick(GameContext &context, Ghost &ghost, float deltaTime) -> GhostStates override {
    if (context.paused) {
      return GhostStates::kScatter;
    }

    //        ghost.Update(deltaTime, context.grid, context, *context.pacman, *context.ghosts[0]);

    if (ghost.IsReSpawning()) {
      return GhostStates::kReSpawn;
    }

    return GhostStates::kScatter;
  }

private:
  Vec2 scatterTarget;
};

struct ScaredState : GhostState {
  auto Enter(GameContext &context, Ghost &ghost) -> void override { ghost.ReSpawn(); }

  auto Tick(GameContext &context, Ghost &ghost, float deltaTime) -> GhostStates override {
    if (context.paused) {
      return GhostStates::kScared;
    }
    return GhostStates::kScared;
  }
};

struct ReSpawnState : GhostState {
  auto Enter(GameContext &context, Ghost &ghost) -> void override { ghost.ReSpawn(); }

  auto Tick(GameContext &context, Ghost &ghost, float deltaTime) -> GhostStates override {
    if (context.paused) {
      return GhostStates::kReSpawn;
    }
    return GhostStates::kReSpawn;
  }
};
