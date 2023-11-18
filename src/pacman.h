#ifndef PACMAN_H
#define PACMAN_H

#include <string>
#include <vector>

#include "SDL.h"

#include "vector2.h"
#include "sprite.h"

class Pacman {
public:
    Pacman(SDL_Renderer *renderer);

    void Update(const float deltaTime);
    void Render(SDL_Renderer *renderer);
    void ProcessInput(const Uint8 *state);

private:
    Vector2<int> position;
    Vector2<int> speed;

    std::unique_ptr<Sprite> sprite;
};

#endif