#ifndef PELLET_H
#define PELLET_H

#include <string>
#include <vector>

#include "SDL.h"

#include "vector2.h"
#include "sprite.h"

class Pellet {
public:
    Pellet(SDL_Renderer *renderer, const Vec2 position);

    void Update(const float deltaTime);
    void Render(SDL_Renderer *renderer);

private:
    Vec2 position;
    Sprite sprite;
};

#endif
