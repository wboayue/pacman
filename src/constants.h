#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "vector2.h"

// =============================================================================
// Direction Enum
// =============================================================================
enum class Direction {
  kNeutral,
  kNorth,
  kSouth,
  kEast,
  kWest,
};

// =============================================================================
// Game Timing
// =============================================================================
static constexpr std::size_t kFramesPerSecond = 60;
static constexpr std::size_t kMilliSecondsPerSecond = 1000;
static constexpr std::size_t kFrameDuration = kMilliSecondsPerSecond / kFramesPerSecond;

// =============================================================================
// Game State Durations (seconds)
// =============================================================================
static constexpr float kReadyStateDuration = 4.0f;
static constexpr float kDyingStateDuration = 2.0f;
static constexpr float kLevelCompleteStateDuration = 2.0f;

// =============================================================================
// Board/Grid
// =============================================================================
static constexpr int kCellSize = 8;
static constexpr int kTunnelRow = 17;
static constexpr float kAspectRatio = 224.0f / 288.0f;

// =============================================================================
// Scoring
// =============================================================================
static constexpr int kPelletPoints = 10;
static constexpr int kEnergizerPoints = 50;
static constexpr int kGhostPoints = 200;
static constexpr int kTotalPellets = 244;

// =============================================================================
// Pacman
// =============================================================================
static constexpr float kMaxSpeed = 75.75757625f;
static constexpr float kPacmanSpeedMultiplier = 0.8f;
static constexpr float kEnergizerDuration = 6.0f;
static constexpr Vec2 kPacmanHomeCell = Vec2{13, 26};
static constexpr Vec2 kPacmanHomePosition =
    Vec2{kPacmanHomeCell.x * kCellSize + (kCellSize / 2.0f), kPacmanHomeCell.y *kCellSize + (kCellSize / 2.0f)};

// =============================================================================
// Ghost Common
// =============================================================================
static constexpr int kGhostFps = 4;
static constexpr int kGhostFrameWidth = 16;
static constexpr float kGhostSpeedMultiplier = 0.73f;
static constexpr float kGhostTunnelSpeedMultiplier = 0.5f;
static constexpr float kPenTop = 17.0f * kCellSize;
static constexpr float kPenBottom = 18.0f * kCellSize;

// Blinky (Red Ghost)
static constexpr Vec2 kBlinkyStartCell = Vec2{14, 14};
static constexpr Vec2 kBlinkyScatterCell = Vec2{24, 0};

// Pinky (Pink Ghost)
static constexpr Vec2 kPinkyStartCell = Vec2{14, 17};
static constexpr Vec2 kPinkyScatterCell = Vec2{2, 0};
static constexpr float kPinkyTargetOffset = 4.0f;

// Inky (Cyan Ghost)
static constexpr Vec2 kInkyStartCell = Vec2{12, 17};
static constexpr Vec2 kInkyScatterCell = Vec2{27, 34};
static constexpr float kInkyPacmanOffset = 2.0f;

// Clyde (Orange Ghost)
static constexpr Vec2 kClydeStartCell = Vec2{16, 17};
static constexpr Vec2 kClydeScatterCell = Vec2{0, 34};
static constexpr float kClydeRelaxDistance = 8.0f;

// =============================================================================
// Audio
// =============================================================================
static constexpr int kAudioFrequency = 44100;
static constexpr int kAudioChannels = 2;
static constexpr int kAudioChunkSize = 2048;

// =============================================================================
// UI/Board Manager
// =============================================================================
static constexpr Vec2 kHighScoreCell = Vec2{9, 0};
static constexpr Vec2 k1UpCell = Vec2{3, 0};
static constexpr Vec2 kScoreCell = Vec2{4, 1};
static constexpr int kLifeSize = 16;
static constexpr int kLifeSpriteFrame = 4;
static constexpr Vec2 kLifeCell = Vec2{4, 34};
static constexpr int kFruitSize = 16;
static constexpr Vec2 kFruitCell = Vec2{24, 34};
static constexpr int kLetterOffset = 1;
static constexpr int kDigitOffset = 31;

#endif
