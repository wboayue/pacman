#ifndef PACMAN_H
#define PACMAN_H

#include <string>
#include <vector>

#include "SDL.h"

#include "sprite.h"

class Pacman {
public:
    Pacman(SDL_Renderer *renderer);

    void Update(const float deltaTime);
    void Render(SDL_Renderer *renderer);
    void ProcessInput(const Uint8 *state);

private:
    int x;
    int y;

    int speed_x;
    int speed_y;

    std::unique_ptr<Sprite> sprite;
};

#endif