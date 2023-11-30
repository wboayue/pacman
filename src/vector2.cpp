#include "vector2.h"

Vec2 Vec2::operator+(const Vec2 &rhs)
{
    return {x+rhs.x, y+rhs.y};
}

Vec2& Vec2::operator+=(const Vec2 &rhs)
{   
    x += rhs.x;
    y += rhs.y;
    
    return *this;
}

bool operator==(const Vec2 &lhs, const Vec2 &rhs) {
    return lhs.x == rhs.x && lhs.y == lhs.y;
};
