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
    SDL_Window* window;
    SDL_GLContext glContext;

    std::unique_ptr<SceneManager> sceneManager;
};
