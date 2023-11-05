#pragma once

#include <cmath>
#include <fmt/core.h>

class Vector4
{
public:
    Vector4()
        : m_x(0.0), m_y(0.0), m_z(0.0), m_w(0.0)
    {
    }

    Vector4(double x, double y, double z, double w)
        : m_x(x), m_y(y), m_z(z), m_w(w)
    {
    }

    Vector4 operator+(Vector4 other) const
    {
        return Vector4(m_x + other.m_x, m_y + other.m_y, m_z + other.m_z, m_w + other.m_w);
    }

    Vector4 operator-(Vector4 other) const
    {
        return Vector4(m_x - other.m_x, m_y - other.m_y, m_z - other.m_z, m_w - other.m_w);
    }

    Vector4 operator*(double f) const
    {
        return Vector4(m_x * f, m_y * f, m_z * f, m_w * f);
    }

    double length() const
    {
        return std::sqrt(m_x * m_x + m_y * m_y + m_z * m_z + m_w * m_w);
    }

    Vector4 normalize() const
    {
        const auto l = length();

        return Vector4(m_x / l, m_y / l, m_z / l, m_w / l);
    }

    Vector4 negate() const
    {
        return Vector4(-m_x, -m_y, -m_z, -m_w);
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

    double getW() const
    {
        return m_w;
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

    void setW(double w)
    {
        m_w = w;
    }

    std::string toString() const
    {
        return fmt::format("Vector4({}, {}, {}, {})", m_x, m_y, m_z, m_w);
    }

private:
    double m_x;
    double m_y;
    double m_z;
    double m_w;
};