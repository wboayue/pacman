#include "board-manager.h"

BoardManager::BoardManager(SDL_Renderer *renderer)
    : maze{renderer, "../assets/maze.png"},
        pacman{renderer, "../assets/pacman.png"},
        fruits{renderer, "../assets/fruits.png"},
        text{renderer, "../assets/white-text.png"}
{
}

void BoardManager::Update(const float deltaTime, GameState &state)
{
    maze.Update(deltaTime);
    
    score = std::to_string(state.score);
}

void BoardManager::Render(SDL_Renderer *renderer)
{
    maze.Render(renderer, {0, 0});

    WriteText(renderer, {9,0}, "HIGH SCORE");
    WriteText(renderer, {3,0}, "1UP");

    WriteText(renderer, {4,1}, score);
}

void BoardManager::WriteText(SDL_Renderer *renderer, Vec2 position, const std::string &text)
{
    SDL_Rect source;
    SDL_Rect destination;

    for (size_t i = 0; i < text.size(); ++i) {
        char c = std::toupper(text[i]);
        
        if (std::isalpha(c)) {
            source.w = 8;
            source.h = 8;
            source.x = (1 + c - 'A') * 8;
            source.y = 0;
        } else if (std::isdigit(c)) {
            source.w = 8;
            source.h = 8;
            source.x = (31 + c - '0') * 8;
            source.y = 0;
        } else {
            source.w = 8;
            source.h = 8;
            source.x = 0;
            source.y = 0;
        }

        destination.w = 8;
        destination.h = 8;
        destination.x = position.x*8 + i*8;
        destination.y = position.y*8;

        this->text.Render(renderer, source, destination);
    }
}
