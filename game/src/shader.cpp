#include "shader.hpp"

std::string readFile(std::string_view path)
{
    std::ifstream file {std::string(path)};
    
    if (!file)
    {
        throw std::runtime_error(std::format("Failed to open file: {}", path));
    }
    
    return std::string(std::istreambuf_iterator<char>(file), {});
}

GLuint compileShader(std::string_view source, GLenum type)
{
    GLuint shader = glCreateShader(type);

    const char* src = source.data();

    glShaderSource(shader, 1, &src, nullptr);

    glCompileShader(shader);

    GLint result;

    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);

    if (!result)
    {
        std::array<GLchar, 512> infoLog;
        glGetShaderInfoLog(shader, infoLog.size(), nullptr, infoLog.data());
        glDeleteShader(shader);
        throw std::runtime_error(std::format("Shader compilation failed: {}", infoLog.data()));
    }

    return shader;
}

Shader::Shader(std::string_view vertexShaderPath, std::string_view fragmentShaderPath)
{
    auto vertexSource = readFile(vertexShaderPath);
    auto vertexShader = compileShader(vertexSource, GL_VERTEX_SHADER);

    if (!vertexShader)
    {
        throw std::runtime_error(std::format("Can not compile vertex shader: {}", vertexShaderPath));
    }

    auto fragmentSource = readFile(fragmentShaderPath);
    auto fragmentShader = compileShader(fragmentSource, GL_FRAGMENT_SHADER);

    if (!fragmentShader)
    {
        glDeleteShader(vertexShader);
        throw std::runtime_error(std::format("Can not compile fragment shader: {}", fragmentShaderPath));
    }

    program = glCreateProgram();

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);

    GLint result;

    glGetProgramiv(program, GL_LINK_STATUS, &result);

    if (!result)
    {
        std::array<GLchar, 512> infoLog;
        glGetProgramInfoLog(program, infoLog.size(), nullptr, infoLog.data());
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(program);
        throw std::runtime_error(std::format("Can not link shader program: {}", infoLog.data()));
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

//Shader::Shader(std::string_view vertexShaderPath, std::string_view fragmentShaderPath, std::string_view geometryShaderPath)
//{
//    auto vertexSource = readFile(vertexShaderPath);
//    auto fragmentSource = readFile(fragmentShaderPath);
//    auto geometrySource = readFile(geometryShaderPath);
//}


void Shader::use() const
{
    glUseProgram(program);
}

void Shader::setVec2(std::string_view name, const glm::vec2& value) const
{
    GLint location = glGetUniformLocation(program, name.data());
    glUniform2fv(location, 1, glm::value_ptr(value));
}

void Shader::setVec3(std::string_view name, const glm::vec3& value) const
{
    GLint location = glGetUniformLocation(program, name.data());
    glUniform3fv(location, 1, glm::value_ptr(value));
}

void Shader::setVec4(std::string_view name, const glm::vec4& value) const
{
    GLint location = glGetUniformLocation(program, name.data());
    glUniform4fv(location, 1, glm::value_ptr(value));
}

void Shader::setMat4(std::string_view name, const glm::mat4& value) const
{
    GLint location = glGetUniformLocation(program, name.data());
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setFloat(std::string_view name, float value) const
{
    GLint location = glGetUniformLocation(program, name.data());
    glUniform1f(location, value);
}

void Shader::setInt(std::string_view name, float value) const
{
    GLint location = glGetUniformLocation(program, name.data());
    glUniform1i(location, value);
}
