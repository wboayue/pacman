#include <iostream>

#include "ghost.h"

static constexpr std::array<Candidate, 4> options{
    Candidate{{0, -1}, Direction::kNorth},
    Candidate{{0, 1}, Direction::kSouth},
    Candidate{{1, 0}, Direction::kEast},
    Candidate{{-1, 0}, Direction::kWest},
};

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
      scatterCell{config.GetScatterCell()}, sprite{config.GetSprite()} {
  position = initialPosition;
  setFramesForHeading(heading);
  currentCell = GetCell();
}

auto Ghost::Update(const float deltaTime, Grid &grid, GameState &state, Pacman &pacman,
                   Ghost &blinky) -> void {
  if (isInPen()) {
    if (active) {
      exitPen();
    } else {
      penDance();
    }
  } else {
    auto target = targeter(*this, pacman, blinky, state.mode);
    chase(grid, target);

    setFramesForHeading(heading);
    setVelocityForHeading(heading);
    position += (velocity * deltaTime);
  }

  // teleport
  if (position.x < -16) {
    position.x = kGridWidth * 8 + 16;
  } else if (position.x > kGridWidth * 8 + 16) {
    position.x = -16;
  }

  sprite->Update(deltaTime);
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
    position.y = floor(((int)position.y / 8) * 8 + 4);
  }
  if (heading == Direction::kNorth || heading == Direction::kSouth) {
    position.x = floor(((int)position.x / 8) * 8 + 4);
  }

  newCell = false;
}

//   position.y = floor(((int)position.y / 8) * 8 + 4);
// }
// if (velocity.y > 0 || velocity.y < 0) {
//   position.x = floor(((int)position.x / 8) * 8 + 4);

auto Ghost::inCellCenter() -> bool {
  float x = position.x - floor(position.x);
  float y = position.y - floor(position.y);

  return (heading == Direction::kNorth && y <= 0.5) || (heading == Direction::kSouth && y >= 0.5) ||
         (heading == Direction::kEast && x >= 0.5) || (heading == Direction::kWest && x <= 0.5);
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
  sprite->Render(renderer, {floor(position.x - 8), floor(position.y - 8)});
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
  auto t = position / 8;
  return {floor(t.x), floor(t.y)};
}

auto Ghost::exitPen() -> void {}

auto Ghost::penDance() -> void {}

// Blinky

BlinkyConfig::BlinkyConfig(SDL_Renderer *renderer) : renderer{renderer} {}

auto BlinkyConfig::GetTargeter() const -> Targeter {
  return [](Ghost &me, Pacman &pacman, [[maybe_unused]] Ghost &blinky, GhostMode mode) {
    if (mode == GhostMode::kScatter || mode == GhostMode::kScared) {
      return me.GetScatterCell();
    }

    return pacman.GetGridPosition();
  };
}

auto BlinkyConfig::GetScatterCell() const -> Vec2 { return Vec2{24, 0}; }

auto BlinkyConfig::GetSprite() const -> std::unique_ptr<Sprite> {
  return std::make_unique<Sprite>(renderer, "../assets/blinky.png", 4, 16);
}

auto BlinkyConfig::GetInitialPosition() const -> Vec2 { return Vec2{14 * 8, 14 * 8 + 4}; }

auto BlinkyConfig::GetInitialHeading() const -> Direction { return Direction::kWest; }

// Inky

InkyConfig::InkyConfig(SDL_Renderer *renderer) : renderer{renderer} {}

auto InkyConfig::GetTargeter() const -> Targeter {
  return [](Ghost &me, Pacman &pacman, Ghost &blinky, GhostMode mode) {
    if (mode == GhostMode::kScatter || mode == GhostMode::kScared) {
      return me.GetScatterCell();
    }
    Vec2 target = pacman.GetGridPosition();
    auto distance = 2.0f;

    if (pacman.GetHeading() == Direction::kNorth) {
      target += Vec2{0, -distance};
    } else if (pacman.GetHeading() == Direction::kSouth) {
      target += Vec2{0, distance};
    } else if (pacman.GetHeading() == Direction::kEast) {
      target += Vec2{distance, 0};
    } else if (pacman.GetHeading() == Direction::kWest) {
      target += Vec2{-distance, 0};
    }

    auto v = target - blinky.GetCell();
    v = v * 2;

    return blinky.GetCell() + v;
  };
};

auto InkyConfig::GetScatterCell() const -> Vec2 { return Vec2{27, 34}; }

auto InkyConfig::GetSprite() const -> std::unique_ptr<Sprite> {
  return std::make_unique<Sprite>(renderer, "../assets/inky.png", 4, 16);
}

auto InkyConfig::GetInitialPosition() const -> Vec2 {
  return Vec2{12 * 8, 14 * 8 + 4};
  //  return Vec2{12*8, 17*8+4};
}

auto InkyConfig::GetInitialHeading() const -> Direction { return Direction::kNorth; }

// Pinky

PinkyConfig::PinkyConfig(SDL_Renderer *renderer) : renderer{renderer} {}

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

auto PinkyConfig::GetScatterCell() const -> Vec2 { return Vec2{2, 0}; }

auto PinkyConfig::GetSprite() const -> std::unique_ptr<Sprite> {
  return std::make_unique<Sprite>(renderer, "../assets/pinky.png", 4, 16);
}

auto PinkyConfig::GetInitialPosition() const -> Vec2 {
  return Vec2{14 * 8, 10 * 8 + 4};
  // return Vec2{14*8, 17*8+4};
}

auto PinkyConfig::GetInitialHeading() const -> Direction { return Direction::kSouth; }

// Clyde

ClydeConfig::ClydeConfig(SDL_Renderer *renderer) : renderer{renderer} {}

auto ClydeConfig::GetTargeter() const -> Targeter {
  return [](Ghost &me, Pacman &pacman, [[maybe_unused]] Ghost &blinky, GhostMode mode) {
    if (mode == GhostMode::kScatter || mode == GhostMode::kScared) {
      return me.GetScatterCell();
    }

    auto d = me.GetCell().Distance(pacman.GetGridPosition());
    if (d > 8.0) {
      return pacman.GetGridPosition();
    } else {
      return me.GetScatterCell();
    }
  };
}

auto ClydeConfig::GetScatterCell() const -> Vec2 { return Vec2{0, 34}; }

auto ClydeConfig::GetSprite() const -> std::unique_ptr<Sprite> {
  return std::make_unique<Sprite>(renderer, "../assets/clyde.png", 4, 16);
}

auto ClydeConfig::GetInitialPosition() const -> Vec2 {
  //  return Vec2{16*8, 17*8+4};
  return Vec2{16 * 8, 12 * 8 + 4};
}

auto ClydeConfig::GetInitialHeading() const -> Direction { return Direction::kNorth; }
