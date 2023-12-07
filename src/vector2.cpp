#include <math.h>

#include "vector2.h"

float Vec2::Distance(const Vec2 &rhs) {
  auto diff = *this - rhs;
  return sqrt(pow(diff.x, 2.0) + pow(diff.y, 2.0));
}

Vec2 Vec2::operator+(const Vec2 &rhs) { return {x + rhs.x, y + rhs.y}; }
Vec2 Vec2::operator-(const Vec2 &rhs) { return {x - rhs.x, y - rhs.y}; }

Vec2 &Vec2::operator+=(const Vec2 &rhs) {
  x += rhs.x;
  y += rhs.y;

  return *this;
}

bool operator==(const Vec2 &lhs, const Vec2 &rhs) {
  return lhs.x == rhs.x && lhs.y == rhs.y;
};
