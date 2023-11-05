#pragma once

#include <cmath>
#include <fmt/core.h>

class Vector3
{
public:
    Vector3()
        : m_x(0.0), m_y(0.0), m_z(0.0)
    {
    }

    Vector3(double x, double y, double z)
        : m_x(x), m_y(y), m_z(z)
    {
    }

    Vector3 operator+(Vector3 other) const
    {
        return Vector3(m_x + other.m_x, m_y + other.m_y, m_z + other.m_z);
    }

    Vector3 operator-(Vector3 other) const
    {
        return Vector3(m_x - other.m_x, m_y - other.m_y, m_z - other.m_z);
    }

    Vector3 operator*(double f) const
    {
        return Vector3(m_x * f, m_y * f, m_z * f);
    }

    double length() const
    {
        return std::sqrt(m_x * m_x + m_y * m_y + m_z * m_z);
    }

    Vector3 normalize() const
    {
        const auto l = length();

        return Vector3(m_x / l, m_y / l, m_z / l);
    }

    Vector3 negate() const
    {
        return Vector3(-m_x, -m_y, -m_z);
    }

    double getX() const
    {
        return m_x;
    }

    double getY() const
    {
        return m_y;
    }

    double getZ() const
    {
        return m_z;
    }

    void setX(double x)
    {
        m_x = x;
    }

    void setY(double y)
    {
        m_y = y;
    }

    void setZ(double z)
    {
        m_z = z;
    }

    std::string toString() const
    {
        return fmt::format("Vector3({}, {}, {})", m_x, m_y, m_z);
    }

private:
    double m_x;
    double m_y;
    double m_z;
};