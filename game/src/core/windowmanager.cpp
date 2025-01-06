#include "windowmanager.hpp"

WindowManager::WindowManager(GLFWwindow* w)
    : window(w) {}

glm::vec2 WindowManager::getWindowSize() const
{
    int w, h;

    glfwGetWindowSize(window, &w, &h);

    return glm::vec2(w, h);
}

glm::vec2 WindowManager::getCursorPosition() const
{
    double w, h;

    glfwGetCursorPos(window, &w, &h);

    return glm::vec2(w, h);
}


void WindowManager::setCursorPosition(glm::vec2 pos) const
{
    glfwSetCursorPos(window, pos.x, pos.y);
}
