#include <iostream>

#include "ghost.h"

static constexpr std::array<Candidate, 4> options{
    Candidate{{0, -1}, Direction::kNorth},
    Candidate{{0, 1}, Direction::kSouth},
    Candidate{{1, 0}, Direction::kEast},
    Candidate{{-1, 0}, Direction::kWest},
};

static constexpr auto kGhostFps = 4;
static constexpr auto kGhostFrameWidth = 16;
static constexpr auto kPenTop = 17.0f * 8.0f;
static constexpr auto kPenBottom = 18.0f * 8.0f;

static constexpr auto kBlinkyStartCell = Vec2{14, 14};
static constexpr auto kBlinkyScatterCell = Vec2{24, 0};

static constexpr auto kClydeStartCell = Vec2{16, 17};
static constexpr auto kClydeScatterCell = Vec2{0, 34};
static constexpr auto kClydeRelaxDistance = 8.0f;

static constexpr auto kPinkyStartCell = Vec2{14, 17};
static constexpr auto kPinkyScatterCell = Vec2{2, 0};

static constexpr auto kInkyStartCell = Vec2{12, 17};
static constexpr auto kInkyScatterCell = Vec2{27, 34};
static constexpr auto kInkyPacmanOffset = 2.0f;

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

Ghost::Ghost(const GhostConfig &config)
    : initialPosition{config.GetInitialPosition()}, heading{config.GetInitialHeading()},
      targeter{config.GetTargeter()},
      scatterCell{config.GetScatterCell()}, sprite{config.GetSprite()}, scaredSprite{config.GetScaredSprite()} {
  position = initialPosition;
  setFramesForHeading(heading);
  currentCell = GetCell();
}

auto Ghost::Update(const float deltaTime, Grid &grid, GameState &state, Pacman &pacman,
                   Ghost &blinky) -> void {
  if (isInPen()) {
    if (active) {
      exitPen(deltaTime);
    } else {
      penDance(deltaTime);
    }
  } else {
    if (pacman.IsEnergized()) {
      mode_ = GhostMode::kScared; 
    } else {
      mode_ = GhostMode::kChase;
    }

    auto target = targeter(*this, pacman, blinky, mode_);
    chase(grid, target);

    setFramesForHeading(heading);
    setVelocityForHeading(heading);
    position += (velocity * deltaTime);
  }

  // teleport
  if (position.x < -(kCellSize * 2)) {
    position.x = kGridWidth * kCellSize + (kCellSize * 2);
  } else if (position.x > kGridWidth * kCellSize + (kCellSize * 2)) {
    position.x = -(kCellSize * 2);
  }

  sprite->Update(deltaTime);
  scaredSprite->Update(deltaTime);
}

auto Ghost::chase(Grid &grid, const Vec2 &target) -> void {
  auto cell = GetCell();
  if (!(currentCell == cell)) {
    currentCell = cell;
    newCell = true;
  }

  if (!(newCell && inCellCenter())) {
    return;
  }

  auto candidates_ = candidates(grid);
  if (candidates_.empty()) {
    heading = reverseDirection(heading);
  } else {
    auto closet = candidates_.at(0);
    for (auto &candidate : candidates_) {
      if (candidate.position.Distance(target) < closet.position.Distance(target)) {
        closet = candidate;
      }
    }
    heading = closet.heading;
  }

  if (heading == Direction::kEast || heading == Direction::kWest) {
    position.y = floor(((int)position.y / kCellSize) * kCellSize + (kCellSize / 2));
  }
  if (heading == Direction::kNorth || heading == Direction::kSouth) {
    position.x = floor(((int)position.x / kCellSize) * kCellSize + (kCellSize / 2));
  }

  newCell = false;
}

auto Ghost::inCellCenter() -> bool {
  static constexpr auto kMidPoint = 0.5f;

  float x = position.x - floor(position.x);
  float y = position.y - floor(position.y);

  return (heading == Direction::kNorth && y <= kMidPoint) ||
         (heading == Direction::kSouth && y >= kMidPoint) ||
         (heading == Direction::kEast && x >= kMidPoint) ||
         (heading == Direction::kWest && x <= kMidPoint);
}

auto Ghost::candidates(Grid &grid) -> std::vector<Candidate> {

  auto results = std::vector<Candidate>{};
  for (auto option : options) {
    if (option.heading == reverseDirection(heading)) {
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
    scaredSprite->Render(renderer, {floor(position.x - kCellSize), floor(position.y - kCellSize)});
  } else {
    sprite->Render(renderer, {floor(position.x - kCellSize), floor(position.y - kCellSize)});
  }
}

auto Ghost::Reset() -> void { position = initialPosition; }

auto Ghost::setFramesForHeading(Direction heading) -> void {
  switch (heading) {
  case Direction::kNorth:
    sprite->SetFrames({4, 5});
    break;

  case Direction::kSouth:
    sprite->SetFrames({6, 7});
    break;

  case Direction::kEast:
    sprite->SetFrames({0, 1});
    break;

  case Direction::kWest:
    sprite->SetFrames({2, 3});
    break;

  default:
    sprite->SetFrames({2, 3});
    break;
  }
}

// velocity for heading
auto Ghost::setVelocityForHeading(Direction heading) -> void {
  float speed = 0.73;
  switch (heading) {
  case Direction::kNorth:
    velocity = Vec2{0, kMaxSpeed * -speed};
    break;

  case Direction::kSouth:
    velocity = Vec2{0, kMaxSpeed * speed};
    break;

  case Direction::kEast:
    velocity = Vec2{kMaxSpeed * speed, 0};
    break;

  case Direction::kWest:
    velocity = Vec2{kMaxSpeed * -speed, 0};
    break;

  default:
    velocity = Vec2{0, 0};
    break;
  }
}

auto Ghost::isInPen() -> bool {
  auto pos = GetCell();
  return pos.x >= 12 && pos.x <= 16 && pos.y >= 17 && pos.y <= 18;
}

auto Ghost::GetCell() -> Vec2 {
  auto t = position / kCellSize;
  return {floor(t.x), floor(t.y)};
}

auto Ghost::exitPen(const float deltaTime) -> void {
  heading = Direction::kNorth;

  setFramesForHeading(heading);
  setVelocityForHeading(heading);

  position += (velocity/2.0 * deltaTime);
}

auto Ghost::penDance(const float deltaTime) -> void {
  setFramesForHeading(heading);
  setVelocityForHeading(heading);

  position += (velocity/2.0 * deltaTime);

  if (position.y < kPenTop) {
    position.y = kPenTop;
    heading = reverseDirection(heading);
  }

  if (position.y > kPenBottom) {
    position.y = kPenBottom;
    heading = reverseDirection(heading);
  }

  setFramesForHeading(heading);
  setVelocityForHeading(heading);
}

// Blinky

BlinkyConfig::BlinkyConfig(SDL_Renderer *renderer) : GhostConfig{renderer} {}

auto BlinkyConfig::GetTargeter() const -> Targeter {
  return [](Ghost &me, Pacman &pacman, [[maybe_unused]] Ghost &blinky, GhostMode mode) {
    if (mode == GhostMode::kScatter || mode == GhostMode::kScared) {
      return me.GetScatterCell();
    }

    return pacman.GetGridPosition();
  };
}

auto BlinkyConfig::GetScatterCell() const -> Vec2 { return kBlinkyScatterCell; }

auto BlinkyConfig::GetSprite() const -> std::unique_ptr<Sprite> {
  return std::make_unique<Sprite>(renderer, "../assets/blinky.png", kGhostFps, kGhostFrameWidth);
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
    Vec2 target = pacman.GetGridPosition();

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
  return std::make_unique<Sprite>(renderer, "../assets/inky.png", kGhostFps, kGhostFrameWidth);
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

    Vec2 target = pacman.GetGridPosition();

    if (pacman.GetHeading() == Direction::kNorth) {
      target += Vec2{0, -4};
    } else if (pacman.GetHeading() == Direction::kSouth) {
      target += Vec2{0, 4};
    } else if (pacman.GetHeading() == Direction::kEast) {
      target += Vec2{4, 0};
    } else if (pacman.GetHeading() == Direction::kWest) {
      target += Vec2{-4, 0};
    }

    return target;
  };
}

auto PinkyConfig::GetScatterCell() const -> Vec2 { return kPinkyScatterCell; }

auto PinkyConfig::GetSprite() const -> std::unique_ptr<Sprite> {
  return std::make_unique<Sprite>(renderer, "../assets/pinky.png", kGhostFps, kGhostFrameWidth);
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

    auto d = me.GetCell().Distance(pacman.GetGridPosition());
    if (d > kClydeRelaxDistance) {
      return pacman.GetGridPosition();
    }
     
    return me.GetScatterCell();
  };
}

auto ClydeConfig::GetScatterCell() const -> Vec2 { return kClydeScatterCell; }

auto ClydeConfig::GetSprite() const -> std::unique_ptr<Sprite> {
  return std::make_unique<Sprite>(renderer, "../assets/clyde.png", kGhostFps, kGhostFrameWidth);
}

auto ClydeConfig::GetInitialPosition() const -> Vec2 {
  return Vec2{kClydeStartCell.x * kCellSize, kClydeStartCell.y * kCellSize + (kCellSize / 2)};
}

auto ClydeConfig::GetInitialHeading() const -> Direction { return Direction::kNorth; }

GhostConfig::GhostConfig(SDL_Renderer *renderer) : renderer{renderer} {
}

auto GhostConfig::GetScaredSprite() const -> std::unique_ptr<Sprite> {
  return std::make_unique<Sprite>(renderer, "../assets/scared-ghost.png", kGhostFps, kGhostFrameWidth);
}
