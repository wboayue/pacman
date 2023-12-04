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

class Ghost {
public:
  Ghost(std::unique_ptr<Sprite> sprite, Vec2 position, Direction heading);

  void Update(const float deltaTime, Grid &grid, GameState &state, Pacman &pacman);
  void Render(SDL_Renderer *renderer);

private:
  
  void setFramesForHeading(Direction heading);
  void setVelocityForHeading(Direction heading);
  bool isInPen();
  void exitPen();
  void penDance();
  Vec2 getGridPosition();
  void chase(Grid &grid, Pacman &pacman);
  Vec2 getTargetGridCell();
  bool inCellCenter();
  std::vector<Candidate> candidates(Grid &grid);

  bool active;
  Vec2 position;
  Vec2 velocity;
  Direction heading;

  Vec2 currentCell;
  bool newCell;

  std::unique_ptr<Sprite> sprite;
};

  // sprite = std::make_unique<Sprite>(renderer, "../assets/pacman.png", 8, 16);
  // sprite->SetFrames({1, 2});

#endif