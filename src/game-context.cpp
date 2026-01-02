#include "game-context.h"
#include "constants.h"

// GhostWaveManager implementation

void GhostWaveManager::Update(float deltaTime) {
  if (paused_) {
    return;
  }

  // Wave 8 (index 7) is indefinite chase
  if (currentWave_ >= 7) {
    return;
  }

  waveTimer_ += deltaTime;
  if (waveTimer_ >= kWaves[currentWave_].second) {
    waveTimer_ = 0.0f;
    currentWave_++;
  }
}

void GhostWaveManager::Pause() { paused_ = true; }

void GhostWaveManager::Resume() { paused_ = false; }

void GhostWaveManager::Reset() {
  currentWave_ = 0;
  waveTimer_ = 0.0f;
  paused_ = false;
}

// GameContext implementation

auto GameContext::NextLevel() -> void {
  pelletsConsumed = 0;
  level += 1;
}

auto GameContext::Reset() -> void {
  score = 0;
  extraLives = 2;
  level = 0;
  pelletsConsumed = 0;
}

auto GameContext::LevelComplete() -> bool { return pelletsConsumed >= kTotalPellets; }
