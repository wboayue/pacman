#ifndef GHOST_H
#define GHOST_H

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

using Targeter = Vec2 (*)(Ghost &me, Pacman &pacman, Ghost &blinky, GhostMode mode);

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

  void Update(const float deltaTime, Grid &grid, GameState &state, Pacman &pacman, Ghost &blinky);
  void Render(SDL_Renderer *renderer);
  void Reset();
  auto ReSpawn() -> void;
  Vec2 GetCell() const;
  Vec2 GetScatterCell() { return scatterCell; };
  void Activate() { active_ = true; };
  auto IsChasing() const -> bool { return mode_ == GhostMode::kChase; };
  auto IsReSpawning() const  -> bool { return mode_ == GhostMode::kReSpawn; }; 
  auto Pause() -> void;
  auto Resume() -> void;

private:
  void setFramesForHeading(Direction heading);
  void setVelocityForHeading(Direction heading);
  bool isInPen();
  void exitPen(const float deltaTime);
  void penDance(const float deltaTime);
  auto moveTowards(Grid &grid, const Vec2 &target) -> Direction;
  bool inCellCenter();
  bool atDecisionPoint(Grid &grid) const;
  std::vector<Candidate> candidates(Grid &grid);
  auto isInTunnel() -> bool;
  auto nextCell(const Direction &direction) const -> Vec2;

  bool active_{false};
  Vec2 position_{};
  Vec2 initialPosition_;
  Vec2 velocity_{};
  Direction heading_;
  Targeter targeter;

  Direction previousHeading_{Direction::kNeutral};
  Vec2 previousCell_{0, 0};
  Vec2 scatterCell{0, 0};

  GhostMode mode_{GhostMode::kChase};

  std::unique_ptr<Sprite> sprite;
  std::unique_ptr<Sprite> scaredSprite;
  std::unique_ptr<Sprite> reSpawnSprite;
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

#endif