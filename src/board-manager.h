#ifndef BOARD_MANAGER_H
#define BOARD_MANAGER_H

#include <string>
#include <vector>

#include "SDL.h"

#include "vector2.h"
#include "sprite.h"

class BoardManager {
public:
    BoardManager(SDL_Renderer *renderer);

    void Update(const float deltaTime);
    void Render(SDL_Renderer *renderer);

private:
    Sprite maze;
    SDL_Renderer *renderer;
};

#endif