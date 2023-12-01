#ifndef SPRITE_H
#define SPRITE_H

#include <string>
#include <vector>

#include "SDL.h"

#include "vector2.h"

class Sprite {
public:
    Sprite(SDL_Renderer *renderer, std::string fileName);
    Sprite(SDL_Renderer *renderer, std::string fileName, int fps, int frameWidth);

    ~Sprite();

    void Update(const float deltaTime);
    void Render(SDL_Renderer *renderer, Vec2 destination);
    void Render(SDL_Renderer *renderer, const SDL_Rect &source, const SDL_Rect &destination);

    void SetFrames(std::vector<int> frames);
    
private:
    SDL_Texture* texture;
    int fps;
    int numFrames;
    int width;
    int height;
    int frameWidth;
    float currentFrame;
    std::vector<int> frames;
};

#endif