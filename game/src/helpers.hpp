#pragma once

#include <iostream>

#include <fmt/core.h>

#define GLFW_INCLUDE_GLEXT
#include <GLFW/glfw3.h>

GLFWwindow* initializeGraphics()
{
    GLFWwindow* window;

    if (!glfwInit())
    {
        std::cerr << fmt::format("Could not initialize GLFW\n");
        return nullptr;
    }

    const char* glsl_version = "#version 460";

#ifdef __APPLE
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // 3.0+ only
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // 3.0+ only
#endif

    auto monitor = glfwGetPrimaryMonitor();

    // int width_mm, height_mm;
    // glfwGetMonitorPhysicalSize(monitor, &width_mm, &height_mm);

    float scale_x, scale_y;
    glfwGetMonitorContentScale(monitor, &scale_x, &scale_y);

    std::cout << fmt::format("Monitor scale: {} x {}\n", scale_x, scale_y);
    std::cout << fmt::format("Window size: {} x {}\n", 1024 * scale_x, 768 * scale_y);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1024 * scale_x, 768 * scale_y, "Hello World", nullptr, nullptr);

    if (!window)
    {
        std::cerr << fmt::format("Could not create GLFW window\n");
        glfwTerminate();
        return nullptr;
    }

    return window;
}
