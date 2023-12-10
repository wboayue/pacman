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

#endif