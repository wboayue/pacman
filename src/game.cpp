#include <iostream>

#include "SDL.h"
#include "SDL_image.h"

#include "game.h"

const int kGameWidth = 224;
const int kGameHeight = 288;

Game::Game() : ready_{false} {
  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
    std::cerr << "SDL could not initialize.\n";
    std::cerr << "SDL_Error: " << SDL_GetError() << "\n";
    return;
  }

  IMG_Init(IMG_INIT_PNG);

  int scale{2};
  renderer_ = std::make_shared<Renderer>(kGameWidth*scale, kGameHeight*scale);
  SDL_RenderSetLogicalSize(renderer_->sdl_renderer, kGameWidth, kGameHeight);

  pacman = std::make_unique<Pacman>(renderer_->sdl_renderer);
  grid = Grid::Load("../assets/maze.txt");

  ready_ = true;
}

Game::~Game() { SDL_Quit(); }

bool Game::Ready() const { return ready_; }

void Game::Run(std::size_t target_frame_duration) {
  Uint32 frame_start;
  Uint32 frame_end;
  Uint32 frame_duration;

  int frame_count = 0;
  running_ = true;

  while (running_) {
    frame_start = SDL_GetTicks();
    float deltaTime = (frame_start - ticks_count_) / 1000.0f;
    ticks_count_ = SDL_GetTicks();

    ProcessInput();
    Update(deltaTime);
    Render();

    frame_end = SDL_GetTicks();

    // Keep track of how long each loop through the input/update/render cycle
    // takes.
    frame_count++;
    frame_duration = frame_end - frame_start;

    // If the time for this frame is too small (i.e. frame_duration is
    // smaller than the target ms_per_frame), delay the loop to
    // achieve the correct frame rate.
    if (frame_duration < target_frame_duration) {
      SDL_Delay(target_frame_duration - frame_duration);
    }
  }
}

void Game::ProcessInput() {
  float aspectRatio = 224.0f / 288.0f;

  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT:
      running_ = false;
      break;
    case SDL_WINDOWEVENT:
      int newWidth = event.window.data1;
      int newHeight = event.window.data2;
      float newAspectRatio = (float)newWidth/(float)newHeight;
      if (newAspectRatio > aspectRatio) {
        newWidth = (int)(newHeight*aspectRatio);
      } else {
        newHeight = (int)(newWidth/aspectRatio);
      }
      renderer_->SetWindowSize(newWidth, newHeight);
    }
  }

  const Uint8 *state = SDL_GetKeyboardState(NULL);
  if (state[SDL_SCANCODE_ESCAPE]) {
    running_ = false;
  }

  pacman->ProcessInput(state);

}

void Game::Update(const float deltaTime) {
  pacman->Update(deltaTime);
}

void Game::Render() {
  renderer_->Clear();

  // move to asset managers
  auto maze = GetTexture("../assets/maze.png");
  SDL_assert(maze != nullptr);

  int width{}, height{};
  SDL_QueryTexture(maze, nullptr, nullptr, &width, &height);

  SDL_Rect r;
  r.w = width;
  r.h = height;
  r.x = 0;
  r.y = 0;

  SDL_RenderCopy(renderer_->sdl_renderer,
  maze,
  nullptr,
  &r);

  pacman->Render(renderer_->sdl_renderer);

  renderer_->Present();
}

SDL_Texture* Game::GetTexture(std::string fileName) {
  SDL_Surface* surface = IMG_Load(fileName.c_str());
  if (!surface) {
    SDL_Log("Failed to load texture file %s", fileName.c_str());
    return nullptr;
  }

  SDL_Texture* texture = renderer_->CreateTextureFromSurface(surface);
  SDL_assert(texture != nullptr);
  SDL_FreeSurface(surface);

  return texture;
}

int Game::GetScore() const { return score; }
