#include "scene2.hpp"

scene2::Scene2::Scene2(WindowManager* windowManager)
    : windowManager(windowManager)
{
    modelShader = std::make_unique<Shader>("resources/shaders/model.vert", "resources/shaders/model.frag");
    model3d = std::make_unique<gltfmodel::GLTFModel>("resources/models/old/Forest1.glb");

    cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::vec3 cameraForward(0.0f, 0.0f, -1.0f);
    cameraPosition = glm::vec3(0.0f, 0.0f, 0.0f);

    cameraOrientation = glm::lookAtRH(cameraPosition, cameraForward, cameraUp);

    fov = 45.0f;
}

scene2::Scene2::~Scene2()
{
    std::cout << "cleaning up scene2" << std::endl;
}

void scene2::Scene2::handleKeyEvent(int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        std::cout << "[scene2] ESC pressed" << std::endl;
    }
}

void scene2::Scene2::handleCursorPositionEvent(double x, double y)
{
    const float cameraRotationSpeed = 1.f;

    glm::vec2 windowSize = windowManager->getWindowSize();

    glm::vec2 mouseDelta = glm::vec2(x, y) - (windowSize * 0.5f);

    glm::quat deltaQuat = glm::quat(glm::vec3(mouseDelta.y / windowSize.x, mouseDelta.x / windowSize.y, 0.0f) * cameraRotationSpeed);

    cameraOrientation = glm::normalize(deltaQuat * cameraOrientation);

    glm::mat4 view = glm::mat4_cast(cameraOrientation) * glm::translate(glm::mat4(1.0f), -glm::vec3(0.0f, 0.0f, -3.0f));

    glm::vec3 forward = glm::vec3(view[0][2], view[1][2], view[2][2]);

    cameraOrientation = glm::lookAtRH(cameraPosition, cameraPosition - forward, cameraUp);

    windowManager->setCursorPosition(windowSize * 0.5f);
}

void scene2::Scene2::render(float dt)
{
    glEnable(GL_DEPTH_TEST);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    modelShader->use();

    glm::mat4 view = glm::mat4_cast(cameraOrientation)  * glm::translate(glm::mat4(1.0f), -glm::vec3(0.0f, 0.0f, -3.0f));

    glm::vec2 windowSize = windowManager->getWindowSize();

    glm::mat4 projection = glm::perspective(glm::radians(fov), (float) windowSize.x / (float) windowSize.y, 0.1f, 1000.0f);

    modelShader->setFloat("dt", dt);
    // modelShader->setMat4("model", model);
    modelShader->setMat4("view", view);
    modelShader->setMat4("projection", projection);

    model3d->render();

    glDisable(GL_DEPTH_TEST);
}
