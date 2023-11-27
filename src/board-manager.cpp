#include "board-manager.h"

BoardManager::BoardManager(SDL_Renderer *renderer)
    :renderer{renderer}, maze{renderer, "../assets/maze.png"}
{
}

void BoardManager::Update(const float deltaTime)
{
    maze.Update(deltaTime);
}

void BoardManager::Render(SDL_Renderer *renderer)
{
    maze.Render(renderer, 0, 0);
}
