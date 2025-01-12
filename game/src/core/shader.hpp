#pragma once

#include "../stdafx.hpp"

class Shader
{
public:
    Shader(std::string_view vertexShaderSource, std::string_view fragmentShaderSource);

    // Shader(std::string_view vertexShaderPath, std::string_view fragmentShaderPath, std::string_view geometryShaderPath);

    // move-only
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    void use() const;

    void setVec2(std::string_view name, const glm::vec2& value) const;
    void setVec2(GLuint location, const glm::vec2& value) const;

    void setVec3(std::string_view name, const glm::vec3& value) const;
    void setVec3(GLuint location, const glm::vec3& value) const;

    void setVec4(std::string_view name, const glm::vec4& value) const;
    void setVec4(GLuint location, const glm::vec4& value) const;

    void setMat4(std::string_view name, const glm::mat4& value) const;
    void setMat4(GLuint location, const glm::mat4& value) const;

    void setFloat(std::string_view name, float value) const;
    void setFloat(GLuint location, float value) const;

    void setInt(std::string_view name, float value) const;
    void setInt(GLuint location, float value) const;

    GLuint getUniformLocation(std::string_view name) const;

    GLuint getSSBOLocation(std::string_view bufferName) const;

    GLuint getSamplerUniformLocation(std::string_view name) const;

    template<typename T>
    void updateBufferData(std::string_view bufferName, const std::vector<T>& data, GLintptr offset = 0) const
    {
        GLuint bufferBinding = getSSBOLocation(bufferName);

        updateBufferData(bufferBinding, data, offset);
    }

    template<typename T>
    void updateBufferData(GLuint binding, const std::vector<T>& data, GLintptr offset = 0) const
    {
        // Get buffer
        GLuint buffer = 0;

        // Get the buffer bound to this binding point
        glGetIntegeri_v(GL_SHADER_STORAGE_BUFFER_BINDING, binding, (GLint*) &buffer);

        if (buffer == 0)
        {
            throw std::runtime_error(fmt::format("No buffer bound for binding {}", binding));
        }

        glNamedBufferSubData(buffer, offset, data.size() * sizeof(T), data.data());
    }

protected:
    GLuint program;
};
