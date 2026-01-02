#include <iostream>

#include "constants.h"
#include "ghost.h"

static constexpr std::array<Candidate, 4> options{
    Candidate{.position = {.x = 0, .y = -1}, .heading = Direction::kNorth},
    Candidate{.position = {.x = 0, .y = 1}, .heading = Direction::kSouth},
    Candidate{.position = {.x = 1, .y = 0}, .heading = Direction::kEast},
    Candidate{.position = {.x = -1, .y = 0}, .heading = Direction::kWest},
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
  return {.x = floor(t.x), .y = floor(t.y)};
}

Ghost::Ghost(const GhostConfig &config)
    : initialPosition_{config.GetInitialPosition()}, heading_{config.GetInitialHeading()},
      targeter_{config.GetTargeter()}, scatterCell_{config.GetScatterCell()}, sprite_{config.GetSprite()},
      scaredSprite_{config.GetScaredSprite()}, respawnSprite_{config.GetReSpawnSprite()} {
  position_ = initialPosition_;
  SetFramesForHeading(heading_);

  // Initialize state based on starting position
  if (IsInPen()) {
    stateType_ = GhostStateType::kPenned;
  } else {
    stateType_ = GhostStateType::kScatter;
  }
  state_ = createState(stateType_);
  state_->Enter(*this);
}

void Ghost::Update(float deltaTime, Grid &grid, GameContext &context, Pacman &pacman, Ghost &blinky,
                   GhostWaveManager &waveManager) {
  UpdateContext ctx{grid, context, pacman, blinky, waveManager};

  auto nextState = state_->Update(*this, deltaTime, ctx);
  if (nextState != stateType_) {
    TransitionTo(nextState);
  }

  sprite_->Update(deltaTime);
  scaredSprite_->Update(deltaTime);
  respawnSprite_->Update(deltaTime);
}

void Ghost::TransitionTo(GhostStateType newState) {
  // Track previous active state for returning from Scared/Respawn
  if (stateType_ == GhostStateType::kChase || stateType_ == GhostStateType::kScatter) {
    previousActiveState_ = stateType_;
  }
  stateType_ = newState;
  state_ = createState(newState);
  state_->Enter(*this);
}

// createState is defined after state classes below

auto Ghost::Pause() -> void {}

auto Ghost::Resume() -> void {}

auto Ghost::NextCell(const Direction &direction) const -> Vec2 {
  auto currentCell = GetCell();

  switch (direction) {
  case Direction::kEast:
    return {.x = currentCell.x + 1, .y = currentCell.y};

  case Direction::kWest:
    return {.x = currentCell.x - 1, .y = currentCell.y};

  case Direction::kNorth:
    return {.x = currentCell.x, .y = currentCell.y - 1};

  case Direction::kSouth:
    return {.x = currentCell.x, .y = currentCell.y + 1};

  default:
    return currentCell;
  }
}

auto Ghost::IsInTunnel() const -> bool {
  auto currentCell = GetCell();

  if (currentCell.y != kTunnelRow) {
    return false;
  }

  return currentCell.x < 4 || currentCell.x > 22;
}

void Ghost::MoveTowards(Grid &grid, const Vec2 &target) {
  auto candidates_ = Candidates(grid);

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

auto Ghost::InCellCenter() const -> bool {
  float centerX = center(position_.x);
  float centerY = center(position_.y);

  return (heading_ == Direction::kNorth && position_.y <= centerY) ||
         (heading_ == Direction::kSouth && position_.y >= centerY) ||
         (heading_ == Direction::kEast && position_.x >= centerX) ||
         (heading_ == Direction::kWest && position_.x <= centerX);
}


auto Ghost::Candidates(Grid &grid) -> std::vector<Candidate> {

  auto results = std::vector<Candidate>{};
  for (auto option : options) {
    if (reverseDirection(option.heading) == previousHeading_) {
      continue;
    }

    auto pos = GetCell() + option.position;
    if (grid.GetCell(pos) == Cell::kWall) {
      continue;
    }

    results.push_back(Candidate{.position = pos, .heading = option.heading});
  }

  return results;
}

void Ghost::Render(SDL_Renderer *renderer) {
  Vec2 renderPos{floor(position_.x - kCellSize), floor(position_.y - kCellSize)};

  if (stateType_ == GhostStateType::kScared) {
    scaredSprite_->Render(renderer, renderPos);
  } else if (stateType_ == GhostStateType::kRespawning) {
    respawnSprite_->Render(renderer, renderPos);
  } else {
    sprite_->Render(renderer, renderPos);
  }
}

void Ghost::Reset() {
  position_ = initialPosition_;
  active_ = false;
  previousHeading_ = Direction::kNeutral;
  previousCell_ = {0, 0};

  if (IsInPen()) {
    stateType_ = GhostStateType::kPenned;
  } else {
    stateType_ = GhostStateType::kScatter;
  }
  state_ = createState(stateType_);
  state_->Enter(*this);
}

void Ghost::SetFramesForHeading(Direction heading) {
  switch (heading) {
  case Direction::kNorth:
    sprite_->SetFrames({4, 5});
    respawnSprite_->SetFrames({2});
    break;

  case Direction::kSouth:
    sprite_->SetFrames({6, 7});
    respawnSprite_->SetFrames({3});
    break;

  case Direction::kEast:
    sprite_->SetFrames({0, 1});
    respawnSprite_->SetFrames({0});
    break;

  case Direction::kWest:
    sprite_->SetFrames({2, 3});
    respawnSprite_->SetFrames({1});
    break;

  default:
    sprite_->SetFrames({2, 3});
    break;
  }
}

auto Ghost::SetVelocityForHeading(Direction heading) -> void {
  switch (heading) {
  case Direction::kNorth:
    velocity_ = Vec2{.x = 0, .y = kMaxSpeed * -kGhostSpeedMultiplier};
    break;

  case Direction::kSouth:
    velocity_ = Vec2{.x = 0, .y = kMaxSpeed * kGhostSpeedMultiplier};
    break;

  case Direction::kEast:
    velocity_ = Vec2{.x = kMaxSpeed * kGhostSpeedMultiplier, .y = 0};
    break;

  case Direction::kWest:
    velocity_ = Vec2{.x = kMaxSpeed * -kGhostSpeedMultiplier, .y = 0};
    break;

  default:
    velocity_ = Vec2{.x = 0, .y = 0};
    break;
  }
}

auto Ghost::IsInPen() const -> bool {
  auto pos = GetCell();
  return pos.x >= 12 && pos.x <= 16 && pos.y >= 17 && pos.y <= 18;
}

auto Ghost::GetCell() const -> Vec2 {
  auto t = position_ / kCellSize;
  return {.x = floor(t.x), .y = floor(t.y)};
}

auto Ghost::ExitPen(float deltaTime) -> void {
  heading_ = Direction::kNorth;

  SetFramesForHeading(heading_);
  SetVelocityForHeading(heading_);

  position_ += (velocity_ / 2.0f * deltaTime);
}

auto Ghost::PenDance(float deltaTime) -> void {
  SetFramesForHeading(heading_);
  SetVelocityForHeading(heading_);

  position_ += (velocity_ / 2.0f * deltaTime);

  if (position_.y < kPenTop) {
    position_.y = kPenTop;
    heading_ = reverseDirection(heading_);
  }

  if (position_.y > kPenBottom) {
    position_.y = kPenBottom;
    heading_ = reverseDirection(heading_);
  }

  SetFramesForHeading(heading_);
  SetVelocityForHeading(heading_);
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
  return std::make_unique<Sprite>(renderer, Sprites::kBlinky, kGhostFps, kGhostFrameWidth);
}

auto BlinkyConfig::GetInitialPosition() const -> Vec2 {
  return Vec2{.x = kBlinkyStartCell.x * kCellSize, .y = kBlinkyStartCell.y * kCellSize + (kCellSize / 2)};
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
      target += Vec2{.x = 0, .y = -kInkyPacmanOffset};
    } else if (pacman.GetHeading() == Direction::kSouth) {
      target += Vec2{.x = 0, .y = kInkyPacmanOffset};
    } else if (pacman.GetHeading() == Direction::kEast) {
      target += Vec2{.x = kInkyPacmanOffset, .y = 0};
    } else if (pacman.GetHeading() == Direction::kWest) {
      target += Vec2{.x = -kInkyPacmanOffset, .y = 0};
    }

    auto projection = target - blinky.GetCell();
    projection = projection * 2;

    return blinky.GetCell() + projection;
  };
};

auto InkyConfig::GetScatterCell() const -> Vec2 { return kInkyScatterCell; }

auto InkyConfig::GetSprite() const -> std::unique_ptr<Sprite> {
  return std::make_unique<Sprite>(renderer, Sprites::kInky, kGhostFps, kGhostFrameWidth);
}

auto InkyConfig::GetInitialPosition() const -> Vec2 {
  return Vec2{.x = kInkyStartCell.x * kCellSize, .y = kInkyStartCell.y * kCellSize + (kCellSize / 2)};
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
      target += Vec2{.x = 0, .y = -kPinkyTargetOffset};
    } else if (pacman.GetHeading() == Direction::kSouth) {
      target += Vec2{.x = 0, .y = kPinkyTargetOffset};
    } else if (pacman.GetHeading() == Direction::kEast) {
      target += Vec2{.x = kPinkyTargetOffset, .y = 0};
    } else if (pacman.GetHeading() == Direction::kWest) {
      target += Vec2{.x = -kPinkyTargetOffset, .y = 0};
    }

    return target;
  };
}

auto PinkyConfig::GetScatterCell() const -> Vec2 { return kPinkyScatterCell; }

auto PinkyConfig::GetSprite() const -> std::unique_ptr<Sprite> {
  return std::make_unique<Sprite>(renderer, Sprites::kPinky, kGhostFps, kGhostFrameWidth);
}

auto PinkyConfig::GetInitialPosition() const -> Vec2 {
  return Vec2{.x = kPinkyStartCell.x * kCellSize, .y = kPinkyStartCell.y * kCellSize + (kCellSize / 2)};
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
  return std::make_unique<Sprite>(renderer, Sprites::kClyde, kGhostFps, kGhostFrameWidth);
}

auto ClydeConfig::GetInitialPosition() const -> Vec2 {
  return Vec2{.x = kClydeStartCell.x * kCellSize, .y = kClydeStartCell.y * kCellSize + (kCellSize / 2)};
}

auto ClydeConfig::GetInitialHeading() const -> Direction { return Direction::kNorth; }

GhostConfig::GhostConfig(SDL_Renderer *renderer) : renderer{renderer} {}

auto GhostConfig::GetScaredSprite() const -> std::unique_ptr<Sprite> {
  return std::make_unique<Sprite>(renderer, Sprites::kScaredGhost, kGhostFps, kGhostFrameWidth);
}

auto GhostConfig::GetReSpawnSprite() const -> std::unique_ptr<Sprite> {
  return std::make_unique<Sprite>(renderer, Sprites::kGhostEyes, kGhostFps, kGhostFrameWidth);
}

// State implementations

class PennedState : public GhostState {
public:
  void Enter(Ghost &ghost) override {}

  auto Update(Ghost &ghost, float deltaTime, const UpdateContext &ctx) -> GhostStateType override {
    ghost.PenDance(deltaTime);

    if (ghost.IsActive()) {
      return GhostStateType::kExitingPen;
    }
    return GhostStateType::kPenned;
  }
};

class ExitingPenState : public GhostState {
public:
  void Enter(Ghost &ghost) override { ghost.SetHeading(Direction::kNorth); }

  auto Update(Ghost &ghost, float deltaTime, const UpdateContext &ctx) -> GhostStateType override {
    ghost.ExitPen(deltaTime);

    if (ghost.GetCell().y < kPenTop / kCellSize) {
      return GhostStateType::kScatter;
    }
    return GhostStateType::kExitingPen;
  }
};

class ChaseState : public GhostState {
public:
  void Enter(Ghost &ghost) override {
    ghost.SetFramesForHeading(ghost.GetHeading());
    ghost.SetVelocityForHeading(ghost.GetHeading());
  }

  auto Update(Ghost &ghost, float deltaTime, const UpdateContext &ctx) -> GhostStateType override {
    // Check for scared transition
    if (ctx.pacman.IsEnergized()) {
      return GhostStateType::kScared;
    }

    // Check wave manager for scatter transition
    if (ctx.waveManager.GetCurrentMode() == GhostMode::kScatter) {
      return GhostStateType::kScatter;
    }

    // Handle movement
    updateMovement(ghost, deltaTime, ctx);

    return GhostStateType::kChase;
  }

private:
  void updateMovement(Ghost &ghost, float deltaTime, const UpdateContext &ctx) {
    if (ghost.IsInTunnel()) {
      ghost.SetPosition(ghost.GetPosition() + ghost.GetVelocity() * deltaTime);
      handleTunnelWrap(ghost);
      return;
    }

    if (ghost.InCellCenter()) {
      auto candidates = ghost.Candidates(ctx.grid);
      if (candidates.size() == 1) {
        ghost.SetHeading(candidates[0].heading);
      } else {
        auto target = ghost.GetTargeter()(ghost, ctx.pacman, ctx.blinky, GhostMode::kChase);
        ghost.MoveTowards(ctx.grid, target);
      }
      ghost.SetFramesForHeading(ghost.GetHeading());
      ghost.SetVelocityForHeading(ghost.GetHeading());
    }

    if (ghost.GetPreviousCell() != ghost.GetCell()) {
      ghost.SetPreviousHeading(ghost.GetHeading());
    }
    ghost.SetPreviousCell(ghost.GetCell());

    ghost.SetPosition(ghost.GetPosition() + ghost.GetVelocity() * deltaTime);
    handleWallCollision(ghost, ctx.grid);
  }

  void handleTunnelWrap(Ghost &ghost) {
    auto pos = ghost.GetPosition();
    if (pos.x < -16) {
      ghost.SetPosition({kGridWidth * kCellSize + 16, pos.y});
    } else if (pos.x > kGridWidth * kCellSize + 16) {
      ghost.SetPosition({-16, pos.y});
    }
  }

  void handleWallCollision(Ghost &ghost, Grid &grid) {
    auto nextPos = ghost.NextCell(ghost.GetHeading());
    if (grid.GetCell(nextPos) == Cell::kWall) {
      auto pos = ghost.GetPosition();
      switch (ghost.GetHeading()) {
      case Direction::kEast:
        ghost.SetPosition({boundUpper(pos.x), pos.y});
        break;
      case Direction::kWest:
        ghost.SetPosition({boundLower(pos.x), pos.y});
        break;
      case Direction::kNorth:
        ghost.SetPosition({pos.x, boundLower(pos.y)});
        break;
      case Direction::kSouth:
        ghost.SetPosition({pos.x, boundUpper(pos.y)});
        break;
      default:
        break;
      }
    }
  }
};

class ScatterState : public GhostState {
public:
  void Enter(Ghost &ghost) override {
    // Reverse direction on entering scatter
    ghost.SetHeading(reverseDirection(ghost.GetHeading()));
    ghost.SetFramesForHeading(ghost.GetHeading());
    ghost.SetVelocityForHeading(ghost.GetHeading());
  }

  auto Update(Ghost &ghost, float deltaTime, const UpdateContext &ctx) -> GhostStateType override {
    // Check for scared transition
    if (ctx.pacman.IsEnergized()) {
      return GhostStateType::kScared;
    }

    // Check wave manager for chase transition
    if (ctx.waveManager.GetCurrentMode() == GhostMode::kChase) {
      return GhostStateType::kChase;
    }

    // Handle movement toward scatter cell
    updateMovement(ghost, deltaTime, ctx);

    return GhostStateType::kScatter;
  }

private:
  void updateMovement(Ghost &ghost, float deltaTime, const UpdateContext &ctx) {
    if (ghost.IsInTunnel()) {
      ghost.SetPosition(ghost.GetPosition() + ghost.GetVelocity() * deltaTime);
      handleTunnelWrap(ghost);
      return;
    }

    if (ghost.InCellCenter()) {
      auto candidates = ghost.Candidates(ctx.grid);
      if (candidates.size() == 1) {
        ghost.SetHeading(candidates[0].heading);
      } else {
        ghost.MoveTowards(ctx.grid, ghost.GetScatterCell());
      }
      ghost.SetFramesForHeading(ghost.GetHeading());
      ghost.SetVelocityForHeading(ghost.GetHeading());
    }

    if (ghost.GetPreviousCell() != ghost.GetCell()) {
      ghost.SetPreviousHeading(ghost.GetHeading());
    }
    ghost.SetPreviousCell(ghost.GetCell());

    ghost.SetPosition(ghost.GetPosition() + ghost.GetVelocity() * deltaTime);
    handleWallCollision(ghost, ctx.grid);
  }

  void handleTunnelWrap(Ghost &ghost) {
    auto pos = ghost.GetPosition();
    if (pos.x < -16) {
      ghost.SetPosition({kGridWidth * kCellSize + 16, pos.y});
    } else if (pos.x > kGridWidth * kCellSize + 16) {
      ghost.SetPosition({-16, pos.y});
    }
  }

  void handleWallCollision(Ghost &ghost, Grid &grid) {
    auto nextPos = ghost.NextCell(ghost.GetHeading());
    if (grid.GetCell(nextPos) == Cell::kWall) {
      auto pos = ghost.GetPosition();
      switch (ghost.GetHeading()) {
      case Direction::kEast:
        ghost.SetPosition({boundUpper(pos.x), pos.y});
        break;
      case Direction::kWest:
        ghost.SetPosition({boundLower(pos.x), pos.y});
        break;
      case Direction::kNorth:
        ghost.SetPosition({pos.x, boundLower(pos.y)});
        break;
      case Direction::kSouth:
        ghost.SetPosition({pos.x, boundUpper(pos.y)});
        break;
      default:
        break;
      }
    }
  }
};

class ScaredState : public GhostState {
public:
  void Enter(Ghost &ghost) override {
    // Reverse direction when becoming scared
    ghost.SetHeading(reverseDirection(ghost.GetHeading()));
    ghost.SetFramesForHeading(ghost.GetHeading());
    ghost.SetVelocityForHeading(ghost.GetHeading());
  }

  auto Update(Ghost &ghost, float deltaTime, const UpdateContext &ctx) -> GhostStateType override {
    // Check if energizer expired
    if (!ctx.pacman.IsEnergized()) {
      return ghost.GetPreviousActiveState();
    }

    // Handle movement (flee toward scatter cell)
    updateMovement(ghost, deltaTime, ctx);

    return GhostStateType::kScared;
  }

private:
  void updateMovement(Ghost &ghost, float deltaTime, const UpdateContext &ctx) {
    if (ghost.IsInTunnel()) {
      ghost.SetPosition(ghost.GetPosition() + ghost.GetVelocity() * deltaTime);
      handleTunnelWrap(ghost);
      return;
    }

    if (ghost.InCellCenter()) {
      auto candidates = ghost.Candidates(ctx.grid);
      if (candidates.size() == 1) {
        ghost.SetHeading(candidates[0].heading);
      } else {
        ghost.MoveTowards(ctx.grid, ghost.GetScatterCell());
      }
      ghost.SetFramesForHeading(ghost.GetHeading());
      ghost.SetVelocityForHeading(ghost.GetHeading());
    }

    if (ghost.GetPreviousCell() != ghost.GetCell()) {
      ghost.SetPreviousHeading(ghost.GetHeading());
    }
    ghost.SetPreviousCell(ghost.GetCell());

    ghost.SetPosition(ghost.GetPosition() + ghost.GetVelocity() * deltaTime);
    handleWallCollision(ghost, ctx.grid);
  }

  void handleTunnelWrap(Ghost &ghost) {
    auto pos = ghost.GetPosition();
    if (pos.x < -16) {
      ghost.SetPosition({kGridWidth * kCellSize + 16, pos.y});
    } else if (pos.x > kGridWidth * kCellSize + 16) {
      ghost.SetPosition({-16, pos.y});
    }
  }

  void handleWallCollision(Ghost &ghost, Grid &grid) {
    auto nextPos = ghost.NextCell(ghost.GetHeading());
    if (grid.GetCell(nextPos) == Cell::kWall) {
      auto pos = ghost.GetPosition();
      switch (ghost.GetHeading()) {
      case Direction::kEast:
        ghost.SetPosition({boundUpper(pos.x), pos.y});
        break;
      case Direction::kWest:
        ghost.SetPosition({boundLower(pos.x), pos.y});
        break;
      case Direction::kNorth:
        ghost.SetPosition({pos.x, boundLower(pos.y)});
        break;
      case Direction::kSouth:
        ghost.SetPosition({pos.x, boundUpper(pos.y)});
        break;
      default:
        break;
      }
    }
  }
};

class RespawningState : public GhostState {
public:
  void Enter(Ghost &ghost) override {
    // Set eyes sprite based on heading
    ghost.SetFramesForHeading(ghost.GetHeading());
  }

  auto Update(Ghost &ghost, float deltaTime, const UpdateContext &ctx) -> GhostStateType override {
    // Move toward initial position at 2x speed
    auto target = toCell(ghost.GetInitialPosition());

    if (ghost.InCellCenter()) {
      auto candidates = ghost.Candidates(ctx.grid);
      if (candidates.size() == 1) {
        ghost.SetHeading(candidates[0].heading);
      } else {
        ghost.MoveTowards(ctx.grid, target);
      }
      ghost.SetFramesForHeading(ghost.GetHeading());
      ghost.SetVelocityForHeading(ghost.GetHeading());
    }

    if (ghost.GetPreviousCell() != ghost.GetCell()) {
      ghost.SetPreviousHeading(ghost.GetHeading());
    }
    ghost.SetPreviousCell(ghost.GetCell());

    // Move at 2x speed
    ghost.SetPosition(ghost.GetPosition() + ghost.GetVelocity() * 2.0f * deltaTime);

    // Handle tunnel wrap
    auto pos = ghost.GetPosition();
    if (pos.x < -16) {
      ghost.SetPosition({kGridWidth * kCellSize + 16, pos.y});
    } else if (pos.x > kGridWidth * kCellSize + 16) {
      ghost.SetPosition({-16, pos.y});
    }

    // Check if arrived at pen
    if (ghost.GetCell() == target) {
      return GhostStateType::kExitingPen;
    }

    return GhostStateType::kRespawning;
  }
};

// Factory function for creating ghost states
auto Ghost::createState(GhostStateType type) -> std::unique_ptr<GhostState> {
  switch (type) {
  case GhostStateType::kPenned:
    return std::make_unique<PennedState>();
  case GhostStateType::kExitingPen:
    return std::make_unique<ExitingPenState>();
  case GhostStateType::kChase:
    return std::make_unique<ChaseState>();
  case GhostStateType::kScatter:
    return std::make_unique<ScatterState>();
  case GhostStateType::kScared:
    return std::make_unique<ScaredState>();
  case GhostStateType::kRespawning:
    return std::make_unique<RespawningState>();
  default:
    return std::make_unique<PennedState>();
  }
}
