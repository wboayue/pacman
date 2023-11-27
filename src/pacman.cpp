#include <iostream>
#include <math.h>

#include "pacman.h"

Pacman::Pacman(SDL_Renderer *renderer, Grid &grid)
    : speed{0, 0}, position{13*8+4, 26*8+4}, grid{grid}
{
 // 12, 25
  sprite = std::make_unique<Sprite>(renderer, "../assets/pacman.png", 4, 16);
  sprite->SetFrames({1, 2});
}

void Pacman::Update(const float deltaTime)
{
    auto nextPosition = NextGridPosition();
    //std::cout << "next " << nextPosition << " -> " <<  position/8 << " - " << grid.GetCell(nextPosition) << std::endl;

    if (grid.GetCell(nextPosition) == Cell::kWall) {
        speed.x = 0;
        speed.y = 0;
    }

    position += (speed * deltaTime);

    if (speed.x > 0 || speed.x < 0) {
        position.y = floor(((int)position.y/8) * 8 +4);
    }
    if (speed.y > 0 || speed.y < 0) {
        position.x = floor(((int)position.x/8) * 8 +4);
    }

    sprite->Update(deltaTime);
}

void Pacman::Render(SDL_Renderer *renderer)
{
    sprite->Render(renderer, (int)position.x-8, (int)position.y-8);
}

void Pacman::ProcessInput(const Uint8 *state)
{
    if (state[SDL_SCANCODE_RIGHT]) {
        if (grid.GetCell(NextGridPosition(Direction::kEast)) != Cell::kWall) {
            sprite->SetFrames({1, 2});
            speed = Vec2{kMaxSpeed*0.8f, 0};
        }
    } else if (state[SDL_SCANCODE_LEFT]) {
        if (grid.GetCell(NextGridPosition(Direction::kWest)) != Cell::kWall) {
            sprite->SetFrames({3, 4});
            speed = Vec2{-kMaxSpeed*0.8f, 0};
        }
    } else if (state[SDL_SCANCODE_UP]) {
        if (grid.GetCell(NextGridPosition(Direction::kNorth)) != Cell::kWall) {
            sprite->SetFrames({5, 6});
            speed = Vec2{0, -kMaxSpeed*0.8f};
        }
    } else if (state[SDL_SCANCODE_DOWN]) {
        if (grid.GetCell(NextGridPosition(Direction::kSouth)) != Cell::kWall) {
            sprite->SetFrames({7, 8});
            speed = Vec2{0, kMaxSpeed*0.8f};
        }
    }
}

Vec2 Pacman::GetPosition()
{
    return position;
}

Vec2 Pacman::GetGridPosition()
{
    auto t = position/8;
    return {t.x, t.y};
}

Vec2 Pacman::NextGridPosition()
{
    auto currentPosition = GetGridPosition();
 
    if (speed.x > 0) {
        return {currentPosition.x+1, currentPosition.y};
    }
    if (speed.x < 0) {
        return {currentPosition.x-1, currentPosition.y};
    }
    if (speed.y > 0) {
        return {currentPosition.x, currentPosition.y+1};
    }
    if (speed.y < 0) {
        return {currentPosition.x, currentPosition.y-1};
    }

    return currentPosition;
}

Vec2 Pacman::NextGridPosition(const Direction &direction)
{
    auto currentPosition = GetGridPosition();
 
    switch (direction)
    {
    case kEast:
        return {currentPosition.x+1, currentPosition.y};

    case kWest:
        return {currentPosition.x-1, currentPosition.y};

    case kNorth:
        return {currentPosition.x, currentPosition.y-1};

    case kSouth:
        return {currentPosition.x, currentPosition.y+1};
    
    default:
        return currentPosition;
    }
}
