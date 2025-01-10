#include "shader.hpp"

static std::string readFile(std::string_view path)
{
    std::ifstream file {std::string(path)};

    if (!file)
    {
        throw std::runtime_error(fmt::format("Failed to open file: {}", path));
    }

    return std::string(std::istreambuf_iterator<char>(file), {});
}

static GLuint compileShader(std::string_view source, GLenum type)
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
        throw std::runtime_error(fmt::format("Shader compilation failed: {}", infoLog.data()));
    }

    return shader;
}

Shader::Shader(std::string_view vertexShaderSource, std::string_view fragmentShaderSource)
{
    auto vertexShader = compileShader(vertexShaderSource, GL_VERTEX_SHADER);

    if (!vertexShader)
    {
        throw std::runtime_error("Can not compile vertex shader");
    }

    auto fragmentShader = compileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

    if (!fragmentShader)
    {
        glDeleteShader(vertexShader);
        throw std::runtime_error("Can not compile fragment shader");
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
        throw std::runtime_error(fmt::format("Can not link shader program: {}", infoLog.data()));
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
    setVec2(location, value);
}

void Shader::setVec3(std::string_view name, const glm::vec3& value) const
{
    GLint location = glGetUniformLocation(program, name.data());
    setVec3(location, value);
}

void Shader::setVec4(std::string_view name, const glm::vec4& value) const
{
    GLint location = glGetUniformLocation(program, name.data());
    setVec4(location, value);
}

void Shader::setMat4(std::string_view name, const glm::mat4& value) const
{
    GLint location = glGetUniformLocation(program, name.data());
    setMat4(location, value);
}

void Shader::setFloat(std::string_view name, float value) const
{
    GLint location = glGetUniformLocation(program, name.data());
    setFloat(location, value);
}

void Shader::setInt(std::string_view name, float value) const
{
    GLint location = glGetUniformLocation(program, name.data());
    setInt(location, value);
}

void Shader::setVec2(GLuint location, const glm::vec2& value) const
{
    glUniform2fv(location, 1, glm::value_ptr(value));
}

void Shader::setVec3(GLuint location, const glm::vec3& value) const
{
    glUniform3fv(location, 1, glm::value_ptr(value));
}

void Shader::setVec4(GLuint location, const glm::vec4& value) const
{
    glUniform4fv(location, 1, glm::value_ptr(value));
}

void Shader::setMat4(GLuint location, const glm::mat4& value) const
{
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setFloat(GLuint location, float value) const
{
    glUniform1f(location, value);
}

void Shader::setInt(GLuint location, float value) const
{
    glUniform1i(location, value);
}

GLuint Shader::getUniformLocation(std::string_view name) const
{
    return glGetUniformLocation(program, name.data());
}

GLuint Shader::getSSBOLocation(std::string_view bufferName) const
{
    // glGetProgramResourceIndex works for any named resource in the shader
    GLuint index = glGetProgramResourceIndex(program, GL_SHADER_STORAGE_BLOCK, bufferName.data());

    if (index == GL_INVALID_INDEX)
    {
        throw std::runtime_error(fmt::format("Failed to find SSBO '{}'", bufferName));
    }

    // Get the buffer's binding point
    GLint binding = 0;
    GLenum props[] = {GL_BUFFER_BINDING};
    GLsizei length;
    glGetProgramResourceiv(program, GL_SHADER_STORAGE_BLOCK, index, 1, props, 1, &length, &binding);

    return binding;
}
