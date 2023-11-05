#pragma once

#include <cmath>
#include <fmt/core.h>
#include <iostream>

#include "../math/Vector3.h"
#include "../math/Matrix4.h"

class Camera
{
public:
    Camera(Vector3 up, Vector3 forward, Vector3 right, Vector3 position)
        : m_up(up), m_forward(forward), m_right(right), m_position(position)
    {
    }

    Matrix4 getViewMatrix() const
    {
        std::cout << "got view matrix from camera\n";
        return Matrix4();
    }

    Matrix4 getProjectionMatrix() const
    {
        std::cout << "got projection matrix from camera\n";
        return Matrix4();
    }

    void rotate(double dx, double dy)
    {
        std::cout << fmt::format("rotated camera by {}, {}\n", dx, dy);
    }

private:
    Vector3 m_up;
    Vector3 m_forward;
    Vector3 m_right;
    Vector3 m_position;
};
