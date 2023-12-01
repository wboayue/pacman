#ifndef RENDERER_H
#define RENDERER_H

#include "SDL.h"
#include "sprite.h"
#include <string>
#include <vector>

class Renderer {
public:
  Renderer(const std::size_t screen_width, const std::size_t screen_height);
  ~Renderer();

  void SetWindowSize(int width, int height);

  void Clear();
  void Present();

  SDL_Texture *CreateTextureFromSurface(SDL_Surface *surface);

  SDL_Renderer *sdl_renderer;

  Sprite *CreateSprite(std::string fileName);

private:
  SDL_Window *sdl_window;
  // SDL_Renderer *sdl_renderer;

  const std::size_t screen_width;
  const std::size_t screen_height;
};

#endif