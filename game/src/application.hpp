#pragma once

#include "stdafx.hpp"

#include "scenemanager.hpp"

#include "scenes/scene1.hpp"
#include "scenes/scene2.hpp"

class Application {
public:
    Application();

    ~Application();

    void update();

    // move-only
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

public:
    bool isRunning;

private:
    GLFWwindow* window;

    std::unique_ptr<SceneManager> sceneManager;
};
