#pragma once

#include <cassert>
#include <cmath>

template <typename T>
class Vector3
{
public:
    Vector3()
        : x(ZERO), y(ZERO), z(ZERO)
    {
    }

    Vector3(T _x, T _y, T _z)
        : x(_x), y(_y), z(_z)
    {
    }

    Vector3(const Vector3<T>& other)
        : x(other.getX()), y(other.getY()), z(other.getZ())
    {
    }

    T getX() const
    {
        return x;
    }

    T getY() const
    {
        return y;
    }

    T getZ() const
    {
        return z;
    }

    Vector3<T> operator+(const Vector3<T>& other) const
    {
        return Vector3<T>(x + other.getX(), y + other.getY(), z + other.getZ());
    }

    Vector3<T> operator*(const T value) const
    {
        return Vector3<T>(x * value, y * value, z * value);
    }

    Vector3<T> operator/(const T value) const
    {
        assert(("the value a vector is divided by must not be zero", value != ZERO));

        return Vector3<T>(x / value, y / value, z / value);
    }

    bool operator==(const Vector3<T>& other) const
    {
        return this->x == other.x && this->y == other.y && this->z == other.z;
    }

    T length() const
    {
        return std::sqrt((x * x) + (y * y) + (z * z));
    }

    Vector3<T> normalize() const
    {
        const auto l = length();

        assert(("the vector being normalized should have non-zero length", l != ZERO));

        return Vector3<T>(x / l, y / l, z / l);
    }

private:
    T x;
    T y;
    T z;

    T ZERO = { 0 };
};

template class Vector3<double>;
