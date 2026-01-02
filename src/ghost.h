#ifndef GHOST_H
#define GHOST_H

#include <memory>

#include "constants.h"
#include "game-context.h"
#include "grid.h"
#include "pacman.h"
#include "sprite.h"
#include "vector2.h"

struct Candidate {
  Vec2 position;
  Direction heading;
};

class Ghost;
class Pacman;
class GhostState;

using Targeter = Vec2 (*)(Ghost &me, Pacman &pacman, Ghost &blinky, GhostMode mode);

enum class GhostStateType {
  kPenned,
  kExitingPen,
  kChase,
  kScatter,
  kScared,
  kRespawning,
};

struct UpdateContext {
  Grid &grid;
  GameContext &context;
  Pacman &pacman;
  Ghost &blinky;
  GhostWaveManager &waveManager;
};

struct GhostConfig {
  GhostConfig(SDL_Renderer *renderer);

  std::unique_ptr<Sprite> GetScaredSprite() const;
  auto GetReSpawnSprite() const -> std::unique_ptr<Sprite>;

  virtual std::unique_ptr<Sprite> GetSprite() const = 0;
  virtual Vec2 GetInitialPosition() const = 0;
  virtual Direction GetInitialHeading() const = 0;
  virtual Targeter GetTargeter() const = 0;
  virtual Vec2 GetScatterCell() const = 0;

protected:
  SDL_Renderer *renderer;
};

class Ghost {
public:
  Ghost(const GhostConfig &config);

  void Update(float deltaTime, Grid &grid, GameContext &context, Pacman &pacman, Ghost &blinky,
              GhostWaveManager &waveManager);
  void Render(SDL_Renderer *renderer);
  void Reset();
  Vec2 GetCell() const;
  Vec2 GetScatterCell() const { return scatterCell_; }
  void Activate() { active_ = true; }
  auto GetStateType() const -> GhostStateType { return stateType_; }
  auto IsScared() const -> bool { return stateType_ == GhostStateType::kScared; }
  auto IsRespawning() const -> bool { return stateType_ == GhostStateType::kRespawning; }
  auto Pause() -> void;
  auto Resume() -> void;

  // State machine
  void TransitionTo(GhostStateType newState);
  auto GetPreviousActiveState() const -> GhostStateType { return previousActiveState_; }

  // Accessors for states
  auto GetPosition() const -> Vec2 { return position_; }
  auto GetHeading() const -> Direction { return heading_; }
  auto GetVelocity() const -> Vec2 { return velocity_; }
  auto GetInitialPosition() const -> Vec2 { return initialPosition_; }
  auto GetTargeter() const -> Targeter { return targeter_; }
  auto IsActive() const -> bool { return active_; }

  // Mutators for states
  void SetPosition(Vec2 pos) { position_ = pos; }
  void SetHeading(Direction dir) { heading_ = dir; }
  void SetVelocity(Vec2 vel) { velocity_ = vel; }
  void SetPreviousHeading(Direction dir) { previousHeading_ = dir; }
  void SetPreviousCell(Vec2 cell) { previousCell_ = cell; }
  auto GetPreviousHeading() const -> Direction { return previousHeading_; }
  auto GetPreviousCell() const -> Vec2 { return previousCell_; }

  // Movement helpers for states
  void SetFramesForHeading(Direction heading);
  void SetVelocityForHeading(Direction heading);
  void ExitPen(float deltaTime);
  void PenDance(float deltaTime);
  void MoveTowards(Grid &grid, const Vec2 &target);
  auto InCellCenter() const -> bool;
  auto IsInPen() const -> bool;
  auto IsInTunnel() const -> bool;
  auto NextCell(const Direction &direction) const -> Vec2;
  auto Candidates(Grid &grid) -> std::vector<Candidate>;

private:
  auto createState(GhostStateType type) -> std::unique_ptr<GhostState>;

  bool active_{false};
  Vec2 position_{};
  Vec2 initialPosition_;
  Vec2 velocity_{};
  Direction heading_;
  Targeter targeter_;

  Direction previousHeading_{Direction::kNeutral};
  Vec2 previousCell_{0, 0};
  Vec2 scatterCell_{0, 0};

  // State machine
  std::unique_ptr<GhostState> state_;
  GhostStateType stateType_{GhostStateType::kPenned};
  GhostStateType previousActiveState_{GhostStateType::kScatter};

  std::unique_ptr<Sprite> sprite_;
  std::unique_ptr<Sprite> scaredSprite_;
  std::unique_ptr<Sprite> respawnSprite_;
};

struct BlinkyConfig : public GhostConfig {
  BlinkyConfig(SDL_Renderer *renderer);

  std::unique_ptr<Sprite> GetSprite() const override;
  Vec2 GetInitialPosition() const override;
  Direction GetInitialHeading() const override;
  Targeter GetTargeter() const override;
  Vec2 GetScatterCell() const override;
};

struct InkyConfig : public GhostConfig {
  InkyConfig(SDL_Renderer *renderer);

  std::unique_ptr<Sprite> GetSprite() const override;
  Vec2 GetInitialPosition() const override;
  Direction GetInitialHeading() const override;
  Targeter GetTargeter() const override;
  Vec2 GetScatterCell() const override;
};

struct PinkyConfig : public GhostConfig {
  PinkyConfig(SDL_Renderer *renderer);

  std::unique_ptr<Sprite> GetSprite() const override;
  Vec2 GetInitialPosition() const override;
  Direction GetInitialHeading() const override;
  Targeter GetTargeter() const override;
  Vec2 GetScatterCell() const override;
};

struct ClydeConfig : public GhostConfig {
  ClydeConfig(SDL_Renderer *renderer);

  std::unique_ptr<Sprite> GetSprite() const override;
  Vec2 GetInitialPosition() const override;
  Direction GetInitialHeading() const override;
  Targeter GetTargeter() const override;
  Vec2 GetScatterCell() const override;
};

struct Blinky : public Ghost {
  Blinky(SDL_Renderer *renderer) : Ghost{BlinkyConfig{renderer}} {};
};

struct Inky : public Ghost {
  Inky(SDL_Renderer *renderer) : Ghost{InkyConfig{renderer}} {};
};

struct Pinky : public Ghost {
  Pinky(SDL_Renderer *renderer) : Ghost{PinkyConfig{renderer}} {};
};

struct Clyde : public Ghost {
  Clyde(SDL_Renderer *renderer) : Ghost{ClydeConfig{renderer}} {};
};

// Base class for ghost states
class GhostState {
public:
  virtual ~GhostState() = default;
  virtual void Enter(Ghost &ghost) = 0;
  virtual auto Update(Ghost &ghost, float deltaTime, const UpdateContext &ctx) -> GhostStateType = 0;
};

#endif