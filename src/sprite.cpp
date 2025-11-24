#include <iostream>

#include "SDL_image.h"

#include "asset-manager.h"
#include "sprite.h"

Sprite::Sprite(SDL_Renderer *renderer, Sprites sprite) : fps{0}, numFrames{1}, currentFrame{0}, frames{0} {
  texture = AssetManager::LoadSpriteTexture(renderer, sprite);
  SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);
  frameWidth = width;
}

Sprite::Sprite(SDL_Renderer *renderer, Sprites sprite, int fps, int frameWidth) : fps{fps}, currentFrame{0}, frames{0} {
  texture = AssetManager::LoadSpriteTexture(renderer, sprite);
  SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);
  this->frameWidth = frameWidth;
  numFrames = width / frameWidth;
}

Sprite::~Sprite() {
  if (texture != nullptr) {
    SDL_DestroyTexture(texture);
  }
}

auto Sprite::SetFrames(std::vector<int> frames) -> void { this->frames = frames; }

auto Sprite::Update(const float deltaTime) -> void {
  if (fps == 0 || frames.size() < 2) {
    return;
  }

  currentFrame += fps * deltaTime;

  while (currentFrame >= frames.size()) {
    currentFrame -= frames.size();
  }
}

auto Sprite::Render(SDL_Renderer *renderer, Vec2 destination) -> void {
  auto frame = static_cast<int>(currentFrame);

  SDL_Rect source;
  source.w = frameWidth;
  source.h = height;
  source.x = frameWidth * frames[frame];
  source.y = 0;

  SDL_Rect destination_;
  destination_.w = frameWidth;
  destination_.h = height;
  destination_.x = destination.x;
  destination_.y = destination.y;

  Render(renderer, source, destination_);
  // SDL_RenderCopy(renderer, texture, &source, &destination);
}

auto Sprite::Render(SDL_Renderer *renderer, const SDL_Rect &source, const SDL_Rect &destination) -> void {
  SDL_RenderCopy(renderer, texture, &source, &destination);
}
