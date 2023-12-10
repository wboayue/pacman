#include <iostream>

#include "SDL.h"
#include "SDL_image.h"

#include "game.h"

const int kGameWidth = 224;
const int kGameHeight = 288;

Game::Game() {
  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
    std::cerr << "SDL could not initialize.\n";
    std::cerr << "SDL_Error:  " << SDL_GetError() << "\n";
    return;
  }

  IMG_Init(IMG_INIT_PNG);

  int scale{2};
  renderer_ = std::make_shared<Renderer>(kGameWidth * scale, kGameHeight * scale);
  SDL_RenderSetLogicalSize(renderer_->sdl_renderer, kGameWidth, kGameHeight);

  board = std::make_unique<BoardManager>(renderer_->sdl_renderer);

  pacman = std::make_unique<Pacman>(renderer_->sdl_renderer);

  auto cells = Grid::Load("../assets/maze.txt");
  grid = Grid{cells};
  grid.CreatePellets(renderer_->sdl_renderer);

  createGhosts(renderer_->sdl_renderer);

  ready_ = true;
}

Game::~Game() { SDL_Quit(); }

auto Game::createGhosts(SDL_Renderer *renderer) -> void {
  // switch to stack
  blinky = std::make_shared<Ghost>(BlinkyConfig{renderer});
  ghosts.push_back(blinky);

  auto inky = std::make_shared<Ghost>(InkyConfig{renderer});
  ghosts.push_back(inky);

  auto pinky = std::make_shared<Ghost>(PinkyConfig{renderer});
  ghosts.push_back(pinky);

  auto clyde = std::make_shared<Ghost>(ClydeConfig{renderer});
  ghosts.push_back(clyde);
}

auto Game::Ready() const -> bool { return ready_; }

auto Game::Run(std::size_t target_frame_duration) -> void {
  Uint32 frame_start{0};
  Uint32 frame_end{0};
  Uint32 frame_duration{0};

  running_ = true;

  ticks_count_ = SDL_GetTicks();

  while (running_) {
    frame_start = SDL_GetTicks();
    float deltaTime = (frame_start - ticks_count_) / 1000.0f;
    ticks_count_ = SDL_GetTicks();

    processInput();
    update(deltaTime);
    render();

    if (state.levelCompleted) {
      SDL_Delay(500);

      // play sound
      grid.Reset(renderer_->sdl_renderer);
      pacman->Reset();
      state.NextLevel();

      for (auto &ghost : ghosts) {
        ghost->Reset();
      }
    }

    bool killed = false;

    for (auto &ghost : ghosts) {
      if (ghost->GetCell() == pacman->GetGridPosition()) {
        pacman->Reset();
        state.extraLives -= 1;
        state.mode = GhostMode::kChase;
        killed = true;
        break;
      }
    }

    if (killed) {
      for (auto &ghost : ghosts) {
        ghost->Reset();
      }

      if (state.extraLives < 0) {
        state.score = 0;
        state.extraLives = 2;
        state.level = 0;
        state.pelletsConsumed = 0;
        state.levelCompleted = false;
      }
    }

    frame_end = SDL_GetTicks();

    // Keep track of how long each loop through the input/update/render
    // cycle takes.
    frame_duration = frame_end - frame_start;

    // If the time for this frame is too small (i.e. frame_duration is
    // smaller than the target ms_per_frame), delay the loop to
    // achieve the correct frame rate.
    if (frame_duration < target_frame_duration) {
      SDL_Delay(target_frame_duration - frame_duration);
    }
  }
}

auto Game::processInput() -> void {
  static constexpr auto aspectRatio = 224.0f / 288.0f;

  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT:
      running_ = false;
      break;
    case SDL_WINDOWEVENT:
      int newWidth = event.window.data1;
      int newHeight = event.window.data2;
      float newAspectRatio = (float)newWidth / (float)newHeight;
      if (newAspectRatio > aspectRatio) {
        newWidth = (int)((float)newHeight * aspectRatio);
      } else {
        newHeight = (int)((float)newWidth / aspectRatio);
      }
      renderer_->SetWindowSize(newWidth, newHeight);
    }
  }

  const Uint8 *state = SDL_GetKeyboardState(nullptr);
  if (state[SDL_SCANCODE_ESCAPE] != 0u) {
    running_ = false;
  }

  pacman->ProcessInput(state);
}

auto Game::update(const float deltaTime) -> void {
  pacman->Update(deltaTime, grid, state);
  for (auto &ghost : ghosts) {
    ghost->Update(deltaTime, grid, state, *pacman, *blinky);
  }

  grid.Update(deltaTime);

  board->Update(deltaTime, state);
}

auto Game::render() -> void {
  renderer_->Clear();

  board->Render(renderer_->sdl_renderer);
  grid.Render(renderer_->sdl_renderer);

  for (auto &ghost : ghosts) {
    ghost->Render(renderer_->sdl_renderer);
  }

  pacman->Render(renderer_->sdl_renderer);

  renderer_->Present();
}

auto Game::GetTexture(const std::string &fileName) const -> SDL_Texture * {
  SDL_Surface *surface = IMG_Load(fileName.c_str());
  if (surface == nullptr) {
    SDL_Log("Failed to load texture file %s", fileName.c_str());
    return nullptr;
  }

  SDL_Texture *texture = renderer_->CreateTextureFromSurface(surface);
  SDL_assert(texture != nullptr);
  SDL_FreeSurface(surface);

  return texture;
}

auto Game::GetScore() const -> int { return score; }
