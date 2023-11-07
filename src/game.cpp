#include "game.h"
#include <iostream>
#include "SDL.h"

Game::Game(): ready_{false} {
  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) < 0) {
    std::cerr << "SDL could not initialize.\n";
    std::cerr << "SDL_Error: " << SDL_GetError() << "\n";
    return;
  }

  renderer_ = std::make_shared<Renderer>(640, 640);
  ready_ = true;
}

Game::~Game() {
  SDL_Quit();
}

bool Game::Ready() const {
  return ready_;
}

void Game::Run(std::size_t target_frame_duration) {
  Uint32 frame_start;
  Uint32 frame_end;
  Uint32 frame_duration;

  int frame_count = 0;
  running_ = true;

  while (running_) {
    frame_start = SDL_GetTicks();

    ProcessInput();
    Update();
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
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT:
        running_ = false;
        break;
    }
  }

  const Uint8* state = SDL_GetKeyboardState(NULL);
  if (state[SDL_SCANCODE_ESCAPE]) {
    running_ = false;
  }
}

void Game::Update() {
}

void Game::Render() {
}

int Game::GetScore() const { return score; }
