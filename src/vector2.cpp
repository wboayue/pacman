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
