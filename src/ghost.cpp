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

Ghost::Ghost(std::unique_ptr<Sprite> sprite_, Vec2 position, Direction heading)
  : sprite{std::move(sprite_)}, position{position}, heading{heading}
{
  setFramesForHeading(heading);
  currentCell = getGridPosition();
}

void Ghost::Update(const float deltaTime, Grid &grid, GameState &state, Pacman &pacman)
{
  if (isInPen()) {
    if (active) {
      exitPen();
    } else {
      penDance();
    }
  } else {
    chase(grid, pacman);
// CHASE—A ghost's objective in chase mode is to find and capture Pac-Man by hunting him down through the maze. Each ghost exhibits unique behavior when chasing Pac-Man, giving them their different personalities: Blinky (red) is very aggressive and hard to shake once he gets behind you, Pinky (pink) tends to get in front of you and cut you off, Inky (light blue) is the least predictable of the bunch, and Clyde (orange) seems to do his own thing and stay out of the way.
// SCATTER—In scatter mode, the ghosts give up the chase for a few seconds and head for their respective home corners. It is a welcome but brief rest—soon enough, they will revert to chase mode and be after Pac-Man again.
// FRIGHTENED    

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

void Ghost::chase(Grid &grid, Pacman &pacman)
{
  auto cell = getGridPosition();
  if (!(currentCell == cell)) {
    currentCell = cell;
    newCell = true;
  }

  if (!(newCell && inCellCenter())) {
    return;
  }

  auto candidates_ = candidates(grid);
  auto candidate = candidates_[0];

  heading = candidate.heading;

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

bool Ghost::inCellCenter()
{
  float x = position.x - floor(position.x);
  float y = position.y - floor(position.y);

  switch (heading)
  {
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

std::vector<Candidate> Ghost::candidates(Grid &grid)
{

  auto results = std::vector<Candidate>{};
  for (auto option : options) {
    if (option.heading == reverseDirection(heading)) {
      continue;
    }
    auto pos = getGridPosition()+option.position;
    if (grid.GetCell(pos) == Cell::kWall) {
      continue;
    }

    results.push_back(
      Candidate{pos, option.heading}
    );
  } 

  return results;
}

Vec2 Ghost::getTargetGridCell()
{
  return Vec2{24, 0};
}

void Ghost::Render(SDL_Renderer *renderer)
{
  sprite->Render(renderer, {floor(position.x - 8), floor(position.y - 8)});
}

void Ghost::setFramesForHeading(Direction heading)
{
  switch (heading)
  {
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

void Ghost::setVelocityForHeading(Direction heading)
{
  switch (heading)
  {
  case Direction::kNorth:
    velocity = Vec2{0, kMaxSpeed * -0.6f};
    break;

  case Direction::kSouth:
    velocity = Vec2{0, kMaxSpeed * 0.6f};
    break;

  case Direction::kEast:
    velocity = Vec2{kMaxSpeed * 0.6f, 0};
    break;

  case Direction::kWest:
    velocity = Vec2{kMaxSpeed * -0.6f, 0};
    break;

  default:
    velocity = Vec2{0, 0};
    break;
  }
}

bool Ghost::isInPen()
{
  auto pos = getGridPosition();
  return pos.x >= 12 &&
         pos.x <= 16 &&
         pos.y >= 17 &&
         pos.y <= 18;
}

Vec2 Ghost::getGridPosition() {
  auto t = position / 8;
  return {floor(t.x), floor(t.y)};
}

void Ghost::exitPen()
{
}

void Ghost::penDance()
{
}


