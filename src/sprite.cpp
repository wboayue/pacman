#include <iostream>

#include "SDL_image.h"
#include "sprite.h"

SDL_Texture* LoadTexture(SDL_Renderer *renderer, std::string fileName);

Sprite::Sprite(SDL_Renderer *renderer, std::string fileName)
    : fps{0}, numFrames{1}, currentFrame{0}, frames{0}
{
    texture = LoadTexture(renderer, fileName);
    SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);
    frameWidth = width;
}

Sprite::Sprite(SDL_Renderer *renderer, std::string fileName, int fps, int frameWidth)
    : fps{fps}, currentFrame{0}, frames{0}
{
    texture = LoadTexture(renderer, fileName);
    SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);
    this->frameWidth = frameWidth;
    numFrames = width / frameWidth;
}

Sprite::~Sprite()
{
    if (texture != nullptr) {
        SDL_DestroyTexture(texture);
    }
}

void Sprite::SetFrames(std::vector<int> frames) {
    this->frames = frames;
}

void Sprite::Update(const float deltaTime) {
    if (fps == 0 || frames.size() < 2) {
        return;
    }

    currentFrame += fps * deltaTime;

    while (currentFrame >= frames.size()) {
        currentFrame -= frames.size();
    }
}

void Sprite::Render(SDL_Renderer *renderer, int x, int y)
{
    auto frame = static_cast<int>(currentFrame);

    SDL_Rect source;
    source.w = frameWidth;
    source.h = height;
    source.x = frameWidth * frames[frame];
    source.y = 0;

    SDL_Rect destination;
    destination.w = frameWidth;
    destination.h = height;
    destination.x = x;
    destination.y = y;

    SDL_RenderCopy(renderer, texture, &source, &destination);
}

SDL_Texture* LoadTexture(SDL_Renderer *renderer, std::string fileName) {
  SDL_Surface* surface = IMG_Load(fileName.c_str());
  if (!surface) {
    SDL_Log("Failed to load texture file %s", fileName.c_str());
    return nullptr;
  }

  SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_assert(texture != nullptr);
  SDL_FreeSurface(surface);

  return texture;
}
