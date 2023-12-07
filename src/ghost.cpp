#include <iostream>

#include "ghost.h"

const std::vector<Candidate> options{
    Candidate{{0, -1}, Direction::kNorth},
    Candidate{{0, 1}, Direction::kSouth},
    Candidate{{1, 0}, Direction::kEast},
    Candidate{{-1, 0}, Direction::kWest},
};

Direction reverseDirection(Direction direction) {
  switch (direction) {
  case Direction::kNorth:
    return Direction::kSouth;
  case Direction::kSouth:
    return Direction::kNorth;
  case Direction::kEast:
    return Direction::kWest;
  case Direction::kWest:
    return Direction::kEast;
  }
  return Direction::kNeutral;
}

Ghost::Ghost(const GhostConfig &config)
    : sprite{config.GetSprite()}, position{config.GetInitialPosition()},
      heading{config.GetInitialHeading()}, targeter{config.GetTargeter()} {
  setFramesForHeading(heading);
  currentCell = GetCell();
}

void Ghost::Update(const float deltaTime, Grid &grid, GameState &state,
                   Pacman &pacman, Ghost &blinky) {
  if (isInPen()) {
    if (active) {
      exitPen();
    } else {
      penDance();
    }
  } else {
    chase(grid, pacman, blinky);
    // CHASE—A ghost's objective in chase mode is to find and capture
    // Pac-Man by hunting him down through the maze. Each ghost exhibits
    // unique behavior when chasing Pac-Man, giving them their different
    // personalities: Blinky (red) is very aggressive and hard to shake once
    // he gets behind you, Pinky (pink) tends to get in front of you and cut
    // you off, Inky (light blue) is the least predictable of the bunch, and
    // Clyde (orange) seems to do his own thing and stay out of the way.
    // SCATTER—In scatter mode, the ghosts give up the chase for a few
    // seconds and head for their respective home corners. It is a welcome
    // but brief rest—soon enough, they will revert to chase mode and be
    // after Pac-Man again. FRIGHTENED

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

void Ghost::chase(Grid &grid, Pacman &pacman, Ghost &blinky) {
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
      auto target = targeter(*this, pacman, blinky);
      if (candidate.position.Distance(target) <
          closet.position.Distance(target)) {
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

  //  std::cout << "choose " << getGridPosition() << std::endl;
  // std::cout << "heading " << heading << std::endl;
  newCell = false;
}

//   position.y = floor(((int)position.y / 8) * 8 + 4);
// }
// if (velocity.y > 0 || velocity.y < 0) {
//   position.x = floor(((int)position.x / 8) * 8 + 4);

bool Ghost::inCellCenter() {
  float x = position.x - floor(position.x);
  float y = position.y - floor(position.y);

  switch (heading) {
  case Direction::kNorth:
    if (y <= 0.5) {
      return true;
    }
    break;

  case Direction::kSouth:
    if (y >= 0.5) {
      return true;
    }
    break;

  case Direction::kEast:
    if (x >= 0.5) {
      return true;
    }
    break;

  case Direction::kWest:
    if (x <= 0.5) {
      return true;
    }
    break;
  }

  return false;
}

std::vector<Candidate> Ghost::candidates(Grid &grid) {

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

// Vec2 Ghost::getTargetGridCell(Pacman &pacman)
// {
//   return pacman.GetGridPosition();
// }

void Ghost::Render(SDL_Renderer *renderer) {
  sprite->Render(renderer, {floor(position.x - 8), floor(position.y - 8)});
}

void Ghost::setFramesForHeading(Direction heading) {
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

void Ghost::setVelocityForHeading(Direction heading) {
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

bool Ghost::isInPen() {
  auto pos = GetCell();
  return pos.x >= 12 && pos.x <= 16 && pos.y >= 17 && pos.y <= 18;
}

Vec2 Ghost::GetCell() {
  auto t = position / 8;
  return {floor(t.x), floor(t.y)};
}

void Ghost::exitPen() {}

void Ghost::penDance() {}

// Blinky

BlinkyConfig::BlinkyConfig(SDL_Renderer *renderer) : renderer{renderer} {}

Targeter BlinkyConfig::GetTargeter() const {
  return [](Ghost &me, Pacman &pacman, Ghost &blinky) {
    return pacman.GetGridPosition();
  };
}

std::unique_ptr<Sprite> BlinkyConfig::GetSprite() const {
  return std::make_unique<Sprite>(renderer, "../assets/blinky.png", 4, 16);
}

Vec2 BlinkyConfig::GetInitialPosition() const {
  return Vec2{14 * 8, 14 * 8 + 4};
}

Direction BlinkyConfig::GetInitialHeading() const { return Direction::kWest; }

// Inky

InkyConfig::InkyConfig(SDL_Renderer *renderer) : renderer{renderer} {}

Targeter InkyConfig::GetTargeter() const {
  return [](Ghost &me, Pacman &pacman, Ghost &blinky) {
    Vec2 target = pacman.GetGridPosition();
    auto position = pacman.GetGridPosition();
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
}

std::unique_ptr<Sprite> InkyConfig::GetSprite() const {
  return std::make_unique<Sprite>(renderer, "../assets/inky.png", 4, 16);
}

Vec2 InkyConfig::GetInitialPosition() const {
  return Vec2{12 * 8, 14 * 8 + 4};
  //  return Vec2{12*8, 17*8+4};
}

Direction InkyConfig::GetInitialHeading() const { return Direction::kNorth; }

// Pinky

PinkyConfig::PinkyConfig(SDL_Renderer *renderer) : renderer{renderer} {}

Targeter PinkyConfig::GetTargeter() const {
  return [](Ghost &me, Pacman &pacman, Ghost &blinky) {
    Vec2 target = pacman.GetGridPosition();
    auto position = pacman.GetGridPosition();

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

std::unique_ptr<Sprite> PinkyConfig::GetSprite() const {
  return std::make_unique<Sprite>(renderer, "../assets/pinky.png", 4, 16);
}

Vec2 PinkyConfig::GetInitialPosition() const {
  return Vec2{14 * 8, 10 * 8 + 4};
  // return Vec2{14*8, 17*8+4};
}

Direction PinkyConfig::GetInitialHeading() const { return Direction::kSouth; }

// Clyde

ClydeConfig::ClydeConfig(SDL_Renderer *renderer) : renderer{renderer} {}

Targeter ClydeConfig::GetTargeter() const {
  return [](Ghost &me, Pacman &pacman, Ghost &blinky) {
    auto d = me.GetCell().Distance(pacman.GetGridPosition());
    if (d > 8.0) {
      return pacman.GetGridPosition();
    } else {
      return Vec2{0, 34};
    }
  };
}

std::unique_ptr<Sprite> ClydeConfig::GetSprite() const {
  return std::make_unique<Sprite>(renderer, "../assets/clyde.png", 4, 16);
}

Vec2 ClydeConfig::GetInitialPosition() const {
  //  return Vec2{16*8, 17*8+4};
  return Vec2{16 * 8, 12 * 8 + 4};
}

Direction ClydeConfig::GetInitialHeading() const { return Direction::kNorth; }
