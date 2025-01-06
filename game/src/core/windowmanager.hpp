#pragma once

#include "../stdafx.hpp"

class WindowManager
{
public:
    WindowManager(GLFWwindow* w);

    glm::vec2 getWindowSize() const;

    glm::vec2 getCursorPosition() const;

    void setCursorPosition(glm::vec2 pos) const;

private:
    GLFWwindow* window;
};
