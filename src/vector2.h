#ifndef VECTOR2_H
#define VECTOR2_H

template <class T>
struct Vector2 {
    T x;
    T y;

    Vector2<T> operator+(const Vector2<T> &rhs);
    Vector2<T>& operator+=(const Vector2<T> &rhs);
};

template <class T>
Vector2<T> Vector2<T>::operator+(const Vector2<T> &rhs)
{
    return {x+rhs.x, y+rhs.y};
}

template <class T>
Vector2<T>& Vector2<T>::operator+=(const Vector2<T> &rhs)
{   
    x += rhs.x;
    y += rhs.y;
    
    return *this;
}

#endif
