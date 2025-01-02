#pragma once

#include "stdafx.hpp"

#include "scenemanager.hpp"
#include "windowmanager.hpp"

class Application {
public:
    Application();

    ~Application();

    void update();

    // move-only
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

private:
    void initWindow();

    void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

    void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

    void windowSizeCallback(GLFWwindow* w, int width, int height);

    void cursorPositionCallback(GLFWwindow* w, double x, double y);

public:
    bool isRunning;

private:
    GLFWwindow* window;

    std::unique_ptr<SceneManager> sceneManager;
    std::unique_ptr<WindowManager> windowManager;
};
