#ifndef VECTOR2_H
#define VECTOR2_H

#include <ostream>

struct Vec2 {
    int x;
    int y;

    Vec2 operator+(const Vec2 &rhs);
    Vec2& operator+=(const Vec2 &rhs);

    friend std::ostream& operator<<(std::ostream& os, const Vec2& rhs) {
        os << "{" << (int)rhs.x << ", " << (int)rhs.y << "}";
        return os;
    }

    Vec2 operator/(const float& rhs) const {
        if (rhs != 0) {
            return {(int)(x/rhs), (int)(y/rhs)};
        } else {
//            std::cerr << "Error: Division by zero!" << std::endl;
            return {0, 0};
        }
    }

};

#endif
