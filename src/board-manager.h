#ifndef BOARD_MANAGER_H
#define BOARD_MANAGER_H

#include <string>

#include "SDL.h"

#include "game-state.h"
#include "sprite.h"
#include "vector2.h"

class BoardManager {
public:
  BoardManager(SDL_Renderer *renderer);

  void Update(const float deltaTime, GameState &state);
  void Render(SDL_Renderer *renderer);

private:
  void WriteText(SDL_Renderer *renderer, Vec2 position,
                 const std::string &text);
  void RenderExtraLives(SDL_Renderer *renderer);
  void RenderFruits(SDL_Renderer *renderer);

  Sprite maze;
  Sprite pacman;
  Sprite fruits;
  Sprite text;

  std::string score;
  int extraLives;
  int level;
};

#endif