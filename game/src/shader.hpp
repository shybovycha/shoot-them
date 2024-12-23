#pragma once

#include "stdafx.hpp"

class Shader
{
public:
    Shader(std::string_view vertexShaderPath, std::string_view fragmentShaderPath);

    // Shader(std::string_view vertexShaderPath, std::string_view fragmentShaderPath, std::string_view geometryShaderPath);

    // move-only
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    void use() const;

    void setVec2(std::string_view name, const glm::vec2& value) const;

    void setVec3(std::string_view name, const glm::vec3& value) const;

    void setVec4(std::string_view name, const glm::vec4& value) const;

    void setMat4(std::string_view name, const glm::mat4& value) const;

    void setFloat(std::string_view name, float value) const;

    void setInt(std::string_view name, float value) const;

private:
    GLuint program;
};
