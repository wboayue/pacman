#ifndef GHOST_H
#define GHOST_H

#include "constants.h"
#include "game-state.h"
#include "grid.h"
#include "pacman.h"
#include "sprite.h"
#include "vector2.h"

struct Candidate {
  Vec2 position;
  Direction heading;
};

class Ghost;

using Targeter = Vec2 (*)(Ghost &me, Pacman &pacman, Ghost &blinky, GhostMode mode);

struct GhostConfig {
  virtual std::unique_ptr<Sprite> GetSprite() const = 0;
  virtual Vec2 GetInitialPosition() const = 0;
  virtual Direction GetInitialHeading() const = 0;
  virtual Targeter GetTargeter() const = 0;
  virtual Vec2 GetScatterCell() const = 0;
};

class Ghost {
public:
  Ghost(const GhostConfig &config);

  void Update(const float deltaTime, Grid &grid, GameState &state, Pacman &pacman, Ghost &blinky);
  void Render(SDL_Renderer *renderer);
  void Reset();
  Vec2 GetCell();
  Vec2 GetScatterCell() { return scatterCell; };
  void Activate() { active = true; }

private:
  void setFramesForHeading(Direction heading);
  void setVelocityForHeading(Direction heading);
  bool isInPen();
  void exitPen(const float deltaTime);
  void penDance(const float deltaTime);
  void chase(Grid &grid, const Vec2 &target);
  bool inCellCenter();
  std::vector<Candidate> candidates(Grid &grid);

  bool active;
  Vec2 position;
  Vec2 initialPosition;
  Vec2 velocity;
  Direction heading;
  Targeter targeter;

  Vec2 currentCell;
  Vec2 scatterCell;
  bool newCell;

  std::unique_ptr<Sprite> sprite;
};

struct BlinkyConfig : public GhostConfig {
  BlinkyConfig(SDL_Renderer *renderer);

  std::unique_ptr<Sprite> GetSprite() const override;
  Vec2 GetInitialPosition() const override;
  Direction GetInitialHeading() const override;
  Targeter GetTargeter() const override;
  Vec2 GetScatterCell() const override;

private:
  SDL_Renderer *renderer;
};

struct InkyConfig : public GhostConfig {
  InkyConfig(SDL_Renderer *renderer);

  std::unique_ptr<Sprite> GetSprite() const override;
  Vec2 GetInitialPosition() const override;
  Direction GetInitialHeading() const override;
  Targeter GetTargeter() const override;
  Vec2 GetScatterCell() const override;

private:
  SDL_Renderer *renderer;
};

struct PinkyConfig : public GhostConfig {
  PinkyConfig(SDL_Renderer *renderer);

  std::unique_ptr<Sprite> GetSprite() const override;
  Vec2 GetInitialPosition() const override;
  Direction GetInitialHeading() const override;
  Targeter GetTargeter() const override;
  Vec2 GetScatterCell() const override;

private:
  SDL_Renderer *renderer;
};

struct ClydeConfig : public GhostConfig {
  ClydeConfig(SDL_Renderer *renderer);

  std::unique_ptr<Sprite> GetSprite() const override;
  Vec2 GetInitialPosition() const override;
  Direction GetInitialHeading() const override;
  Targeter GetTargeter() const override;
  Vec2 GetScatterCell() const override;

private:
  SDL_Renderer *renderer;
};

#endif