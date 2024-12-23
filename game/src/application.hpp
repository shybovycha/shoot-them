#pragma once

#include "stdafx.hpp"

#include "scenemanager.hpp"

struct Application {
    bool isRunning;

    SDL_Window* window;
    SDL_GLContext glContext;

    std::unique_ptr<SceneManager> sceneManager;

    void init();

    void update();

    void cleanup();
};
