#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include <string>

#include "SDL.h"
#include "SDL_mixer.h"

#include "sprite.h"

class AssetManager {
public:
  AssetManager(const std::string &dataPath) : dataPath{dataPath} {};

  auto GetSound(const std::string &asset) -> Mix_Chunk *;
  auto CreateSprite(const std::string &asset, int frameWidth, int fps) -> Sprite;

  auto GetImagePath(const std::string &asset) -> std::string { return dataPath + asset; }

  auto GetImage(const std::string &asset) -> std::string { return dataPath + asset; }

private:
  std::string dataPath;
  SDL_Renderer *renderer;
};

#endif
