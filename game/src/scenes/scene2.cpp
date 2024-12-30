#include "scene2.hpp"

scene2::Scene2::Scene2()
{
    modelShader = std::make_unique<Shader>("resources/shaders/model.vert", "resources/shaders/model.frag");
    model3d = std::make_unique<gltfmodel::GLTFModel>("resources/models/old/Forest1.glb");
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

void scene2::Scene2::render(float dt)
{
    glEnable(GL_DEPTH_TEST);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    modelShader->use();

    glm::mat4 model = glm::translate(glm::rotate(glm::mat4(1.0f), dt, glm::vec3(0.0f, 1.0f, 0.0f)), glm::vec3(0.0f, 0.0f, -0.5f));
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));

    // TODO: expose projection on a higher level maybe? or obtain actual window size?
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float) windowWidth / (float) windowHeight, 0.1f, 1000.0f);

    modelShader->setFloat("dt", dt);
    modelShader->setMat4("model", model);
    modelShader->setMat4("view", view);
    modelShader->setMat4("projection", projection);

    model3d->render();

    glDisable(GL_DEPTH_TEST);
}
