#include "scene1.hpp"

scene1::Scene1::Scene1(WindowManager* windowManager, SceneManager* sceneManager)
    : Scene(windowManager, sceneManager)
{
    forwardRenderPassShader = std::make_unique<deferredrendering::shaders::ForwardRenderPassShader>();
    shadingRenderPassShader = std::make_unique<deferredrendering::shaders::ShadingRenderPassShader>();

    sceneModel = std::make_unique<gltfmodel::GLTFModel>("resources/models/old/Forest1.glb");
    rifleModel = std::make_unique<gltfmodel::GLTFModel>("resources/models/old/Rifle2.glb");

    for (const auto& l : sceneModel->getLights())
    {
        lights.push_back(deferredrendering::shaders::Light { .color = l.color, .position = l.position, .intensity = l.intensity, .range = l.range });
    }

    // somehow vector iterators are from different vectors here?
    /*lights.insert(
        lights.end(),
        std::make_move_iterator(sceneModel->getLights().begin()),
        std::make_move_iterator(sceneModel->getLights().end())
    );*/

    cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::vec3 cameraForward(0.0f, 0.0f, -1.0f);
    cameraPosition = glm::vec3(0.0f, 0.0f, 0.0f);

    cameraOrientation = glm::lookAtRH(cameraPosition, cameraForward, cameraUp);

    fov = 45.0f;

    // setup screen space quad for deferred rendering
    {
        // 3 floats for position, 2 floats for UV, 4 vertices for a quad
        std::array<float, (3 + 2) * 4> quadVertices[] = {
                // positions        // texture Coords
                -1.0f, 1.0f, 0.0f,  0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                1.0f, 1.0f, 0.0f,   1.0f, 1.0f,
                1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
        };

        // Setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

        // Position attribute
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) 0);

        // TexCoords attribute
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3 * sizeof(float)));

        // Cleanup
        glBindVertexArray(0);
    }

    // setup deferred rendering buffers
    {
        unsigned int width = windowManager->getWindowSize().x;
        unsigned int height = windowManager->getWindowSize().y;

        glCreateBuffers(1, &lightsBuffer);
        glNamedBufferStorage(lightsBuffer, lights.size() * sizeof(deferredrendering::shaders::Light),
                             lights.data(), GL_DYNAMIC_STORAGE_BIT);

        glGenFramebuffers(1, &gBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

        // Position buffer
        glGenTextures(1, &gPositionTexture);
        glBindTexture(GL_TEXTURE_2D, gPositionTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPositionTexture, 0);

        // Normal buffer
        glGenTextures(1, &gNormalTexture);
        glBindTexture(GL_TEXTURE_2D, gNormalTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormalTexture, 0);

        // Albedo + Specular buffer
        glGenTextures(1, &gAlbedoSpecTexture);
        glBindTexture(GL_TEXTURE_2D, gAlbedoSpecTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpecTexture, 0);

        // Tell OpenGL which color attachments we'll use
        GLuint attachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
        glDrawBuffers(3, attachments);

        // Create and attach depth buffer
        glGenRenderbuffers(1, &rboDepthBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, rboDepthBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepthBuffer);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            throw std::runtime_error("Framebuffer is not complete");
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
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
    // forward rendering pass
    {
        glEnable(GL_DEPTH_TEST);

        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        forwardRenderPassShader->use();

        {
            glm::mat4 view = glm::mat4_cast(cameraOrientation) * glm::translate(glm::mat4(1.0f), -glm::vec3(0.0f, 0.0f, -3.0f));

            glm::vec2 windowSize = windowManager->getWindowSize();

            glm::mat4 projection = glm::perspective(glm::radians(fov), (float) windowSize.x / (float) windowSize.y, 0.1f, 1000.0f);

            glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, 0.0f));

            forwardRenderPassShader->set_modelMatrix(modelMatrix);
            forwardRenderPassShader->set_viewMatrix(view);
            forwardRenderPassShader->set_projectionMatrix(projection);

            sceneModel->render();
        }

        // render rifle
        {
            glm::mat4 view = glm::mat4(1.0f);
            glm::vec3 forward = glm::vec3(view[0][2], view[1][2], view[2][2]);

            glm::vec2 windowSize = windowManager->getWindowSize();

            glm::mat4 projection = glm::perspective(glm::radians(fov), (float) windowSize.x / (float) windowSize.y, 0.1f, 1000.0f);

            glm::mat4 modelMatrix = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.15f, -0.2f, -0.8f)), glm::vec3(0.5f));

            forwardRenderPassShader->set_modelMatrix(modelMatrix);
            forwardRenderPassShader->set_viewMatrix(view);
            forwardRenderPassShader->set_projectionMatrix(projection);

            rifleModel->render();
        }
    }

    // shading rendering pass
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shadingRenderPassShader->use();

        shadingRenderPassShader->bindPositionTexture(gPositionTexture);
        shadingRenderPassShader->bindNormalTexture(gNormalTexture);
        shadingRenderPassShader->bindAlbedoSpecTexture(gAlbedoSpecTexture);

        shadingRenderPassShader->set_viewPos(cameraPosition);

        shadingRenderPassShader->set_lights(lights, lightsBuffer);

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
    }
}
