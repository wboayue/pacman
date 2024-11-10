#ifndef CONSTANTS_H
#define CONSTANTS_H

enum class Direction {
  kNeutral,
  kNorth,
  kSouth,
  kEast,
  kWest,
};

static constexpr float kMaxSpeed = 75.75757625;
static constexpr int kCellSize = 8;
static constexpr int kTunnelRow = 17;

static constexpr int kPelletPoints = 10;
static constexpr int kEnergizerPoints = 50;
static constexpr int kTotalPellets = 244;

#endif