#ifndef PACMAN_H
#define PACMAN_H

#include <string>
#include <vector>

#include "SDL.h"

#include "vector2.h"
#include "sprite.h"
#include "grid.h"

enum Direction {
    kNeutral,
    kNorth,
    kSouth,
    kEast,
    kWest,
};

const float kMaxSpeed = 75.75757625;

class Pacman {
public:
    Pacman(SDL_Renderer *renderer, Grid &grid);

    void Update(const float deltaTime);
    void Render(SDL_Renderer *renderer);
    void ProcessInput(const Uint8 *state);

    Vec2 GetPosition();
    Vec2 GetGridPosition();
    Vec2 NextGridPosition(const Direction &direction);
    Vec2 NextGridPosition();

private:
    Vec2 position;
    Vec2 velocity;
    Direction heading;

    Grid &grid;
    std::unique_ptr<Sprite> sprite;
};

#endif