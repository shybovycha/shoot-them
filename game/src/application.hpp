#pragma once

#include "stdafx.hpp"

#include "scenemanager.hpp"

class Application {
public:
    Application();

    ~Application();

    void update();

    // move-only
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

private:
    void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

    void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

public:
    bool isRunning;

private:
    GLFWwindow* window;

    std::unique_ptr<SceneManager> sceneManager;
};
