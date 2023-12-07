#include "renderer.h"
#include "SDL_image.h"

#include <iostream>
#include <string>

Renderer::Renderer(const std::size_t screen_width, const std::size_t screen_height)
    : screen_width(screen_width), screen_height(screen_height) {
  // Create Window
  sdl_window =
      SDL_CreateWindow("Pacman", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screen_width,
                       screen_height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

  if (nullptr == sdl_window) {
    std::cerr << "Window could not be created.\n";
    std::cerr << " SDL_Error: " << SDL_GetError() << "\n";
  }

  // Create renderer
  sdl_renderer = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_ACCELERATED);
  if (nullptr == sdl_renderer) {
    std::cerr << "Renderer could not be created.\n";
    std::cerr << "SDL_Error: " << SDL_GetError() << "\n";
  }
}

Renderer::~Renderer() { SDL_DestroyWindow(sdl_window); }

void Renderer::SetWindowSize(int width, int height) {
  SDL_SetWindowSize(sdl_window, width, height);
}

void Renderer::Clear() { SDL_RenderClear(sdl_renderer); }

void Renderer::Present() { SDL_RenderPresent(sdl_renderer); }

SDL_Texture *Renderer::CreateTextureFromSurface(SDL_Surface *surface) {
  return SDL_CreateTextureFromSurface(sdl_renderer, surface);
}

Sprite *Renderer::CreateSprite(std::string fileName) {
  auto surface = IMG_Load("../assets/maze.png");
  SDL_assert(surface != nullptr);

  auto texture = SDL_CreateTextureFromSurface(sdl_renderer, surface);
  SDL_assert(texture != nullptr);
  int width{}, height{};
  SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);

  return nullptr;
}
