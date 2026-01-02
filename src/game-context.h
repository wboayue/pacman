#ifndef GAME_CONTEXT_H
#define GAME_CONTEXT_H

#include <array>
#include <utility>

enum class GhostMode {
  kScatter,
  kChase,
  kScared,
  kReSpawn,
};

/// Manages global Scatter/Chase wave timing for all ghosts.
/// Follows original Pacman wave pattern: Scatter->Chase cycles.
class GhostWaveManager {
public:
  void Update(float deltaTime);
  void Pause();
  void Resume();
  void Reset();
  auto GetCurrentMode() const -> GhostMode { return paused_ ? GhostMode::kScared : kWaves[currentWave_].first; }
  auto IsPaused() const -> bool { return paused_; }

private:
  static constexpr std::array<std::pair<GhostMode, float>, 8> kWaves{{
      {GhostMode::kScatter, 7.0f},
      {GhostMode::kChase, 20.0f},
      {GhostMode::kScatter, 7.0f},
      {GhostMode::kChase, 20.0f},
      {GhostMode::kScatter, 5.0f},
      {GhostMode::kChase, 20.0f},
      {GhostMode::kScatter, 5.0f},
      {GhostMode::kChase, -1.0f}, // -1 = indefinite
  }};

  int currentWave_{0};
  float waveTimer_{0.0f};
  bool paused_{false};
};

struct GameContext {
  int score{0};
  int extraLives{2};
  int level{0};
  int pelletsConsumed{0};
  bool paused{false};

  // pacman, ghosts
  // audio system

  auto NextLevel() -> void;
  auto Reset() -> void;
  auto LevelComplete() -> bool;
};

#endif