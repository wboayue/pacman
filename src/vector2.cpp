#include <cmath>
#include <limits>

#include "vector2.h"

// Computes distance between vectors.
auto Vec2::Distance(const Vec2 &rhs) const -> float {
  auto diff = *this - rhs;
  return sqrt(pow(diff.x, 2.0) + pow(diff.y, 2.0));
}

// Creates string representation of the vector
auto operator<<(std::ostream &os, const Vec2 &rhs) -> std::ostream & {
  os << "{" << (int)rhs.x << ", " << (int)rhs.y << "}";
  return os;
}

// Calculates the sum of two vectors.
auto Vec2::operator+(const Vec2 &rhs) const -> Vec2 { return {x + rhs.x, y + rhs.y}; }

// Calculates the difference between two vectors.
auto Vec2::operator-(const Vec2 &rhs) const -> Vec2 { return {x - rhs.x, y - rhs.y}; }

// Adds another vector and updates self.
auto Vec2::operator+=(const Vec2 &rhs) -> Vec2 & {
  x += rhs.x;
  y += rhs.y;
  return *this;
}

// Divides this vector by another.
auto Vec2::operator/(const float &rhs) const -> Vec2 {
  if (rhs != 0) {
    return {(x / rhs), (y / rhs)};
  } else {
    return {std::numeric_limits<float>::infinity(), 0};
  }
}

// Computes the product of two vectors.
auto Vec2::operator*(const float &rhs) const -> Vec2 { return {x * rhs, y * rhs}; }

// Checks two vectors for equality
auto operator==(const Vec2 &lhs, const Vec2 &rhs) -> bool {
  return lhs.x == rhs.x && lhs.y == rhs.y;
};
