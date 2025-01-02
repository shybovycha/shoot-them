#include "application.hpp"

Application::Application()
{
    initWindow();

    windowManager = std::make_unique<WindowManager>(window);

    sceneManager = std::make_unique<SceneManager>();

    sceneManager->addScene(SceneID::SCENE1, std::make_unique<scene1::Scene1>(windowManager.get()));
    sceneManager->addScene(SceneID::SCENE2, std::make_unique<scene2::Scene2>(windowManager.get()));

    sceneManager->setScene(SceneID::SCENE2);

    isRunning = true;
}

Application::~Application()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Application::initWindow()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        throw std::runtime_error("Initialization failed");
    }

    glfwSetErrorCallback([](int error, const char* description) { std::cerr << "GLFW error:" << description << std::endl; });

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

    glfwSetCursorPos(window, WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f);

    glfwSetWindowUserPointer(window, this);

    glfwSetKeyCallback(window, [](GLFWwindow* w, int key, int scancode, int action, int mods) { static_cast<Application*>(glfwGetWindowUserPointer(w))->keyCallback(w, key, scancode, action, mods); });

    glfwSetMouseButtonCallback(window, [](GLFWwindow* w, int button, int action, int mods) { static_cast<Application*>(glfwGetWindowUserPointer(w))->mouseButtonCallback(w, button, action, mods); });

    glfwSetWindowSizeCallback(window, [](GLFWwindow* w, int width, int height) { static_cast<Application*>(glfwGetWindowUserPointer(w))->windowSizeCallback(w, width, height); });

    glfwSetCursorPosCallback(window, [](GLFWwindow* w, double x, double y) { static_cast<Application*>(glfwGetWindowUserPointer(w))->cursorPositionCallback(w, x, y); });
}

void Application::keyCallback(GLFWwindow* w, int key, int scancode, int action, int mods)
{
    sceneManager->currentScene->handleKeyEvent(key, scancode, action, mods);
}

void Application::mouseButtonCallback(GLFWwindow* w, int button, int action, int mods)
{
    sceneManager->currentScene->handleMouseButtonEvent(button, action, mods);
}

void Application::windowSizeCallback(GLFWwindow* w, int width, int height)
{
    sceneManager->currentScene->handleWindowResizeEvent(width, height);
}

void Application::cursorPositionCallback(GLFWwindow* w, double x, double y)
{
    sceneManager->currentScene->handleCursorPositionEvent(x, y);
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
