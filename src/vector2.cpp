#include <cmath>

#include "vector2.h"

auto Vec2::Distance(const Vec2 &rhs) const -> float {
  auto diff = *this - rhs;
  return sqrt(pow(diff.x, 2.0) + pow(diff.y, 2.0));
}

auto Vec2::operator+(const Vec2 &rhs) const -> Vec2 { return {x + rhs.x, y + rhs.y}; }
auto Vec2::operator-(const Vec2 &rhs) const -> Vec2 { return {x - rhs.x, y - rhs.y}; }

auto Vec2::operator+=(const Vec2 &rhs) -> Vec2 & {
  x += rhs.x;
  y += rhs.y;
  return *this;
}

auto Vec2::operator/(const float &rhs) const -> Vec2 {
  if (rhs != 0) {
    return {(x / rhs), (y / rhs)};
  } else {
    //            std::cerr << "Error: Division by zero!" << std::endl;
    return {0, 0};
  }
}
auto Vec2::operator*(const float &rhs) const -> Vec2 { return {x * rhs, y * rhs}; }

auto operator==(const Vec2 &lhs, const Vec2 &rhs) -> bool {
  return lhs.x == rhs.x && lhs.y == rhs.y;
};
