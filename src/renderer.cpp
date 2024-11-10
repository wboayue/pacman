#include "renderer.h"
#include "SDL_image.h"

#include <iostream>
#include <string>

Renderer::Renderer(const std::size_t screen_width, const std::size_t screen_height) {
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

auto Renderer::Clear() -> void { SDL_RenderClear(sdl_renderer); }

auto Renderer::Present() -> void { SDL_RenderPresent(sdl_renderer); }

auto Renderer::CreateTextureFromSurface(SDL_Surface *surface) -> SDL_Texture * {
  return SDL_CreateTextureFromSurface(sdl_renderer, surface);
}

auto Renderer::CreateSprite(std::string fileName) -> Sprite * {
  auto surface = IMG_Load(fileName.c_str());
  SDL_assert(surface != nullptr);

  auto texture = SDL_CreateTextureFromSurface(sdl_renderer, surface);
  SDL_assert(texture != nullptr);
  int width{}, height{};
  SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);

  return nullptr;
}
