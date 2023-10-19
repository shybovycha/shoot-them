#pragma once

#include <format>
#include <iostream>

#include "../math/Vector2.h"
#include "../math/Vector3.h"
#include "../math/Matrix3.h"
#include "../math/Matrix4.h"

class Renderer
{
public:
    void useShaderProgram(const std::string& resourceName)
    {
        std::cout << std::format("bind shader '{}'\n", resourceName);
    }

    void useFramebuffer(const std::string& resourceName)
    {
        std::cout << std::format("bind framebuffer '{}'\n", resourceName);
    }

    void setUniform(const std::string& variableName, Vector2 value)
    {
        std::cout << std::format("set uniform '{}' to <vec2>({}, {})\n", variableName, value.getX(), value.getY());
    }

    void setUniform(const std::string& variableName, Vector3 value)
    {
        std::cout << std::format("set uniform '{}' to <vec3>({}, {}, {})\n", variableName, value.getX(), value.getY(), value.getZ());
    }

    void setUniform(const std::string& variableName, Matrix3 value)
    {
        std::cout << std::format("set uniform '{}' to <mat3>()\n", variableName);
    }

    void setUniform(const std::string& variableName, Matrix4 value)
    {
        std::cout << std::format("set uniform '{}' to <mat4>()\n", variableName);
    }

    void setTexture(const std::string& variableName, const std::string& resourceName)
    {
        std::cout << std::format("set uniform '{}' to <texture>({})\n", variableName, resourceName);
    }

    void draw3DModel(const std::string& resourceName)
    {
        std::cout << std::format("draw VAO {}\n", resourceName);
    }
};
