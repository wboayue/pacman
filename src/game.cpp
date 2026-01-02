#include <iostream>

#include "SDL.h"
#include "SDL_image.h"

#include "audio-system.h"
#include "constants.h"
#include "game.h"
#include <map>

const int kGameWidth = 224;
const int kGameHeight = 288;

// The following classes model the game state and game state machine.

enum class GameStates { kReady, kPlay, kPaused, kDying, kLevelComplete };

// Base state interface
class GameState {
public:
  virtual ~GameState() = default;

  // Core state methods
  virtual auto Enter(Game &game) -> void {};
  virtual auto Tick(Game &game, float deltaTime) -> GameStates = 0;
};

auto initializeStates() -> std::map<GameStates, std::unique_ptr<GameState>>;

Game::Game(AssetManager &assetManager) : assetManager{assetManager}, audio{assetManager} {
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
  blinky = std::make_shared<Ghost>(BlinkyConfig{renderer});
  blinky->Activate();
  ghosts.push_back(blinky);

  auto inky = std::make_shared<Ghost>(InkyConfig{renderer});
  ghosts.push_back(inky);

  auto pinky = std::make_shared<Ghost>(PinkyConfig{renderer});
  pinky->Activate();
  ghosts.push_back(pinky);

  auto clyde = std::make_shared<Ghost>(ClydeConfig{renderer});
  ghosts.push_back(clyde);
}

auto Game::Ready() const -> bool { return ready_; }

auto Game::Run(std::size_t target_frame_duration) -> void {
  Uint32 frame_start{0};
  Uint32 frame_end{0};
  Uint32 frame_duration{0};

  auto currentState = GameStates::kReady;
  auto states = initializeStates();

  states[currentState]->Enter(*this);

  ticks_count_ = SDL_GetTicks();
  running_ = true;

  while (running_) {
    frame_start = SDL_GetTicks();
    float deltaTime = static_cast<float>(frame_start - ticks_count_) / 1000.0f;
    ticks_count_ = SDL_GetTicks();

    auto nextState = states[currentState]->Tick(*this, deltaTime);
    if (nextState != currentState) {
      currentState = nextState;
      states[currentState]->Enter(*this);
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

auto Game::processInput() -> const Uint8 * {
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
      if (newAspectRatio < kAspectRatio) {
        newWidth = (int)((float)newHeight * kAspectRatio);
      } else {
        newHeight = (int)((float)newWidth / kAspectRatio);
      }
      renderer_->SetWindowSize(newWidth, newHeight);
    }
  }

  const Uint8 *state = SDL_GetKeyboardState(nullptr);
  if (state[SDL_SCANCODE_ESCAPE] != 0u) {
    running_ = false;
  }

  return state;
}

auto Game::update(const float deltaTime) -> void {
  updateEntities(deltaTime);
  updateAnimations(deltaTime);
}

auto Game::updateEntities(const float deltaTime) -> void {
  waveManager_.Update(deltaTime);
  pacman->Update(deltaTime, grid, context, audio, ghosts);
  for (auto &ghost : ghosts) {
    ghost->Update(deltaTime, grid, context, *pacman, *blinky, waveManager_);
  }
}

auto Game::updateAnimations(const float deltaTime) -> void {
  grid.Update(deltaTime);
  board->Update(deltaTime, context);
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

auto Game::Pause() -> void {
  pacman->Pause();
  for (auto &ghost : ghosts) {
    ghost->Pause();
  }
}

auto Game::Resume() -> void {
  pacman->Resume();
  for (auto &ghost : ghosts) {
    ghost->Resume();
  }
}

auto Game::PlaySound(Sounds sound) -> void {
  [[maybe_unused]] auto [handle, future] = audio.PlaySound(sound, std::nullopt);
}

// The following classes implement the game state machine.
// Valid transitions are:
// Ready -> Play
// Play -> Paused, Dying, LevelComplete
// Paused -> Play
// Dying -> Ready
// LevelComplete -> Ready

struct ReadyState : GameState {
  auto Enter(Game &game) -> void override {
    elapsedTime = 0.0f;
    game.pacman->Reset();
    game.waveManager_.Reset();
    game.PlaySound(Sounds::kIntro);
  }

  auto Tick(Game &game, float deltaTime) -> GameStates override {
    game.processInput();
    game.updateAnimations(deltaTime);
    game.render();

    if (elapsedTime >= kReadyStateDuration) {
      return GameStates::kPlay;
    }

    elapsedTime += deltaTime;

    return GameStates::kReady;
  }

private:
  float elapsedTime{0.0f};
};

struct PlayState : GameState {
  auto Tick(Game &game, float deltaTime) -> GameStates override {
    auto keyState = game.processInput();
    game.pacman->ProcessInput(keyState);

    game.update(deltaTime);
    game.render();

    if (pauseRequested(keyState)) {
      return GameStates::kPaused;
    } else if (levelCompleted(game)) {
      return GameStates::kLevelComplete;
    } else if (wasKilled(game)) {
      return GameStates::kDying;
    } else {
      return GameStates::kPlay;
    }
  }

private:
  auto pauseRequested(const Uint8 *keyState) const -> bool { return keyState[SDL_SCANCODE_P] != 0u; }

  auto levelCompleted(Game &game) const -> bool { return game.context.LevelComplete(); }

  auto wasKilled(Game &game) const -> bool {
    for (auto &ghost : game.ghosts) {
      if (ghost->CanKill() && (ghost->GetCell() == game.pacman->GetCell())) {
        return true;
      }
    }
    return false;
  }
};

struct PausedState : GameState {

  auto Enter(Game &game) -> void override { pause(game); }

  auto Tick(Game &game, float deltaTime) -> GameStates override {
    auto keyState = game.processInput();
    game.update(deltaTime);
    game.render();

    if (resumeRequested(keyState)) {
      resume(game);
      return GameStates::kPlay;
    } else {
      return GameStates::kPaused;
    }
  }

private:
  auto pause(Game &game) const -> void {
    game.pacman->Pause();
    for (auto &ghost : game.ghosts) {
      ghost->Pause();
    }
  }

  auto resume(Game &game) const -> void {
    game.pacman->Resume();
    for (auto &ghost : game.ghosts) {
      ghost->Resume();
    }
  }

  auto resumeRequested(const Uint8 *keyState) const -> bool { return keyState[SDL_SCANCODE_P] != 0u; }
};

struct DyingState : GameState {
  auto Enter(Game &game) -> void override {
    std::cout << "Entering Dying State\n";
    elapsedTime = 0.0f;
    game.PlaySound(Sounds::kDeath);
    game.context.extraLives -= 1;
  }

  auto Tick(Game &game, float deltaTime) -> GameStates override {
    game.processInput();
    game.updateAnimations(deltaTime);
    game.render();

    if (elapsedTime >= kDyingStateDuration) {
      reset(game);

      if (game.context.extraLives < 0) {
        return GameStates::kReady;
      }

      return GameStates::kPlay;
    }

    elapsedTime += deltaTime;

    return GameStates::kDying;
  }

private:
  auto reset(Game &game) const -> void {
    game.pacman->Reset();

    for (auto &ghost : game.ghosts) {
      ghost->Reset();
    }
  }

  float elapsedTime{0.0f};
};

struct LevelCompleteState : GameState {
  auto Enter(Game &game) -> void override {
    std::cout << "Entering Level Complete State\n";
    elapsedTime = 0.0f;
    game.audio.CancelAllSounds();
    // play sound
  }

  auto Tick(Game &game, float deltaTime) -> GameStates override {
    game.processInput();
    game.updateAnimations(deltaTime);
    game.render();

    if (elapsedTime > kLevelCompleteStateDuration) {
      completeLevel(game);
      return GameStates::kReady;
    }

    elapsedTime += deltaTime;

    return GameStates::kLevelComplete;
  }

private:
  auto completeLevel(Game &game) const -> void {
    game.grid.Reset(game.renderer_->sdl_renderer);
    game.pacman->Reset();
    game.waveManager_.Reset();
    game.context.NextLevel();

    for (auto &ghost : game.ghosts) {
      ghost->Reset();
    }
  }

  float elapsedTime{0.0f};
};

auto initializeStates() -> std::map<GameStates, std::unique_ptr<GameState>> {
  auto states = std::map<GameStates, std::unique_ptr<GameState>>{};

  states.emplace(GameStates::kReady, std::make_unique<ReadyState>());
  states.emplace(GameStates::kPlay, std::make_unique<PlayState>());
  states.emplace(GameStates::kPaused, std::make_unique<PausedState>());
  states.emplace(GameStates::kDying, std::make_unique<DyingState>());
  states.emplace(GameStates::kLevelComplete, std::make_unique<LevelCompleteState>());

  return states;
}
