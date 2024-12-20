#include <iostream>

#include <fmt/core.h>

#include "helpers.hpp"
#include "scene.hpp"

#include "scenes/scene1.hpp"

int main()
{
    GLFWwindow* window = initializeGraphics();

    if (window == nullptr)
    {
        std::cerr << fmt::format("Could not get window instance\n");
        return -1;
    }

    std::vector<std::shared_ptr<Scene>> scenes{
        std::make_unique<Scene1>()
    };

    auto currentScene = scenes.begin();
    auto previousTime = glfwGetTime();
    auto currentTime = previousTime;

    std::cout << fmt::format("Current scene: {}\n", currentScene != scenes.end());

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Poll for and process events */
        glfwPollEvents();

        /* Render here */
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);

        glClear(GL_COLOR_BUFFER_BIT);

        currentTime = glfwGetTime();
        float dt = currentTime - previousTime;
        previousTime = currentTime;

        (*currentScene)->render(dt);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
