#ifndef POWER_PELLET_H
#define POWER_PELLET_H

#include <string>
#include <vector>

#include "SDL.h"

#include "vector2.h"
#include "sprite.h"

class PowerPellet {
public:
    PowerPellet(SDL_Renderer *renderer);

    void Update(const float deltaTime);
    void Render(SDL_Renderer *renderer);
    void ProcessInput(const Uint8 *state);

private:
    Vector2<int> position;
    Sprite sprite;
};

#endif