#pragma once

#include <iostream>
#include <fmt/format.h>

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

struct Application {
    bool isRunning;

    SDL_Window* window;
    SDL_GLContext glContext;

    void init();

    void update();

    void close();
};
