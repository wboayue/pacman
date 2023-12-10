#ifndef VECTOR2_H
#define VECTOR2_H

#include <functional>
#include <ostream>

struct Vec2 {
  float x;
  float y;

  auto Distance(const Vec2 &rhs) const -> float;

  auto operator+(const Vec2 &rhs) const -> Vec2;
  auto operator-(const Vec2 &rhs) const -> Vec2;
  auto operator+=(const Vec2 &rhs) -> Vec2 &;

  friend auto operator<<(std::ostream &os, const Vec2 &rhs) -> std::ostream & {
    os << "{" << (int)rhs.x << ", " << (int)rhs.y << "}";
    return os;
  }

  auto operator/(const float &rhs) const -> Vec2;
  auto operator*(const float &rhs) const -> Vec2;
};

struct Vec2Hash {
  size_t operator()(const Vec2 &item) const {
    size_t h1 = std::hash<float>()(item.x);
    size_t h2 = std::hash<float>()(item.y);
    return h1 ^ (h2 << 1);
  }
};

auto operator==(const Vec2 &lhs, const Vec2 &rhs) -> bool;

#endif
