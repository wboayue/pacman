#ifndef VECTOR2_H
#define VECTOR2_H

#include <ostream>
#include <functional>

struct Vec2 {
    float x;
    float y;

    Vec2 operator+(const Vec2 &rhs);
    Vec2& operator+=(const Vec2 &rhs);

    friend std::ostream& operator<<(std::ostream& os, const Vec2& rhs) {
        os << "{" << (int)rhs.x << ", " << (int)rhs.y << "}";
        return os;
    }

    Vec2 operator/(const float& rhs) const {
        if (rhs != 0) {
            return {(x/rhs), (y/rhs)};
        } else {
//            std::cerr << "Error: Division by zero!" << std::endl;
            return {0, 0};
        }
    }
    Vec2 operator*(const float& rhs) const {
        return {x*rhs, y*rhs};
    }

};

struct Vec2Hash {
    size_t operator()(const Vec2 &item) const {
        size_t h1 = std::hash<float>()(item.x);
        size_t h2 = std::hash<float>()(item.y);
        return h1 ^ (h2 << 1);
    }
};

bool operator==(const Vec2 &lhs, const Vec2 &rhs);

#endif
