#include "application.hpp"

void errorCallback(int error, const char* description)
{
    std::cerr << "GLFW error:" << description << std::endl;
}

Application::Application()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        throw std::runtime_error("Initialization failed");
    }

    glfwSetErrorCallback(errorCallback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    const auto WINDOW_WIDTH = 1024;
    const auto WINDOW_HEIGHT = 768;

    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "ShootThem!", NULL, NULL);

    if (!window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        throw std::runtime_error("Initialization failed");
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        throw std::runtime_error("Initialization failed");
    }

    glfwSwapInterval(0);

    // glfwSetKeyCallback(window, key_callback);

    sceneManager = std::make_unique<SceneManager>();

    sceneManager->addScene(SceneID::SCENE1, std::make_unique<scene1::Scene1>());
    sceneManager->addScene(SceneID::SCENE2, std::make_unique<scene2::Scene2>());

    sceneManager->setScene(SceneID::SCENE2);

    isRunning = true;
}

Application::~Application()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Application::update()
{
    if (glfwWindowShouldClose(window)) {
        isRunning = false;
    }

    double dt = glfwGetTime();

    int framebufferWidth, framebufferHeight;
    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);

    glViewport(0, 0, framebufferWidth, framebufferHeight);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    sceneManager->currentScene->render(dt);

    glfwSwapBuffers(window);
    glfwPollEvents();
}
