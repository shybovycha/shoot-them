#include "scene1.hpp"

scene1::Scene1::Scene1(WindowManager* windowManager, SceneManager* sceneManager)
    : Scene(windowManager, sceneManager)
{
    modelShader = std::make_unique<Shader>("resources/shaders/model.vert", "resources/shaders/model.frag");
    sceneModel = std::make_unique<gltfmodel::GLTFModel>("resources/models/old/Forest1.glb");

    cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::vec3 cameraForward(0.0f, 0.0f, -1.0f);
    cameraPosition = glm::vec3(0.0f, 0.0f, 0.0f);

    cameraOrientation = glm::lookAtRH(cameraPosition, cameraForward, cameraUp);

    fov = 45.0f;
}

scene1::Scene1::~Scene1()
{
    std::cout << "cleaning up scene1" << std::endl;
}

void scene1::Scene1::handleKeyEvent(int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        std::cout << "[scene2] ESC pressed" << std::endl;
    }

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        sceneManager->setScene(SceneID::SCENE2);
    }
}

void scene1::Scene1::handleCursorPositionEvent(double x, double y)
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

void scene1::Scene1::render(float dt)
{
    glEnable(GL_DEPTH_TEST);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    modelShader->use();

    glm::mat4 view = glm::mat4_cast(cameraOrientation)  * glm::translate(glm::mat4(1.0f), -glm::vec3(0.0f, 0.0f, -3.0f));

    glm::vec2 windowSize = windowManager->getWindowSize();

    glm::mat4 projection = glm::perspective(glm::radians(fov), (float) windowSize.x / (float) windowSize.y, 0.1f, 1000.0f);

    glm::mat4 modelMatrix = glm::mat4(1.0f); // TODO: change this for each model

    modelShader->setFloat("dt", dt);
    modelShader->setMat4("model", modelMatrix);
    modelShader->setMat4("view", view);
    modelShader->setMat4("projection", projection);

    sceneModel->render();

    glDisable(GL_DEPTH_TEST);
}
