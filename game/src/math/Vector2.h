#pragma once

#include <cmath>
#include <fmt/core.h>

class Vector2
{
public:
    Vector2()
        : m_x(0.0), m_y(0.0)
    {
    }

    Vector2(double x, double y)
        : m_x(x), m_y(y)
    {
    }

    Vector2 operator+(Vector2 other) const
    {
        return Vector2(m_x + other.m_x, m_y + other.m_y);
    }

    Vector2 operator-(Vector2 other) const
    {
        return Vector2(m_x - other.m_x, m_y - other.m_y);
    }

    Vector2 operator*(double f) const
    {
        return Vector2(m_x * f, m_y * f);
    }

    double length() const
    {
        return std::sqrt(m_x * m_x + m_y * m_y);
    }

    Vector2 normalize() const
    {
        const auto l = length();

        return Vector2(m_x / l, m_y / l);
    }

    Vector2 negate() const
    {
        return Vector2(-m_x, -m_y);
    }

    double getX() const
    {
        return m_x;
    }

    double getY() const
    {
        return m_y;
    }

    void setX(double x)
    {
        m_x = x;
    }

    void setY(double y)
    {
        m_y = y;
    }

    std::string toString() const
    {
        return fmt::format("Vector2({}, {})", m_x, m_y);
    }

private:
    double m_x;
    double m_y;
};
