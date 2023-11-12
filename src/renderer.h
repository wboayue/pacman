#ifndef RENDERER_H
#define RENDERER_H

#include "SDL.h"
#include <vector>

class Renderer {
public:
  Renderer(const std::size_t screen_width, const std::size_t screen_height);
  ~Renderer();

  void SetWindowSize(int width, int height);

  SDL_Renderer *sdl_renderer;

private:
  SDL_Window *sdl_window;
  // SDL_Renderer *sdl_renderer;

  const std::size_t screen_width;
  const std::size_t screen_height;
};

#endif