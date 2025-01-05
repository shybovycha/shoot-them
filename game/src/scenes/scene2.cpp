#include "scene2.hpp"

scene2::Scene2::Scene2(WindowManager* windowManager, SceneManager* sceneManager)
    : Scene(windowManager, sceneManager)
{
    modelShader = std::make_unique<Shader>("resources/shaders/model.vert", "resources/shaders/model.frag");
    sceneModel = std::make_unique<gltfmodel::GLTFModel>("resources/models/old/Egypt2.glb");
    rifleModel = std::make_unique<gltfmodel::GLTFModel>("resources/models/old/Rifle2.glb");

    cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::vec3 cameraForward(0.0f, 0.0f, -1.0f);
    cameraPosition = glm::vec3(0.0f, 0.25f, 0.0f);

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

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        sceneManager->setScene(SceneID::SCENE1);
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

    {
        glm::mat4 view = glm::mat4_cast(cameraOrientation) * glm::translate(glm::mat4(1.0f), -glm::vec3(0.0f, 0.0f, -3.0f));

        glm::vec2 windowSize = windowManager->getWindowSize();

        glm::mat4 projection = glm::perspective(glm::radians(fov), (float) windowSize.x / (float) windowSize.y, 0.1f, 1000.0f);

        glm::mat4 modelMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        modelShader->setFloat("dt", dt);
        modelShader->setMat4("model", modelMatrix);
        modelShader->setMat4("view", view);
        modelShader->setMat4("projection", projection);

        sceneModel->render();
    }

    // glDisable(GL_DEPTH_TEST);

    // render rifle
    {
        glm::mat4 view = glm::mat4(1.0f);
        glm::vec3 forward = glm::vec3(view[0][2], view[1][2], view[2][2]);

        glm::vec2 windowSize = windowManager->getWindowSize();

        glm::mat4 projection = glm::perspective(glm::radians(fov), (float) windowSize.x / (float) windowSize.y, 0.1f, 1000.0f);

        glm::mat4 modelMatrix = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.15f, -0.2f, -0.8f)), glm::vec3(0.5f));

        modelShader->setFloat("dt", dt);
        modelShader->setMat4("model", modelMatrix);
        modelShader->setMat4("view", view);
        modelShader->setMat4("projection", projection);

        rifleModel->render();
    }
}
