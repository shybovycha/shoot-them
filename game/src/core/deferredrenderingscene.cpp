#include "deferredrenderingscene.hpp"

DeferredRenderingScene::DeferredRenderingScene(WindowManager* windowManager, SceneManager* sceneManager)
    : Scene(windowManager, sceneManager)
{
    forwardRenderPassShader = std::make_unique<deferredrendering::shaders::ForwardRenderPassShader>();
    shadingRenderPassShader = std::make_unique<deferredrendering::shaders::ShadingRenderPassShader>();

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

DeferredRenderingScene::~DeferredRenderingScene()
{
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);

    glDeleteFramebuffers(1, &gBuffer);
    
    glDeleteTextures(1, &gPositionTexture);
    glDeleteTextures(1, &gNormalTexture);
    glDeleteTextures(1, &gAlbedoSpecTexture);

    glDeleteRenderbuffers(1, &rboDepthBuffer);
}

void DeferredRenderingScene::render(float dt)
{
    // first render pass
    {
        glEnable(GL_DEPTH_TEST);

        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        forwardRenderPassShader->use();

        // delegate
        forwardRenderPass(dt, forwardRenderPassShader.get());
    }

    // second render pass
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shadingRenderPassShader->use();

        shadingRenderPassShader->bindPositionTexture(gPositionTexture);
        shadingRenderPassShader->bindNormalTexture(gNormalTexture);
        shadingRenderPassShader->bindAlbedoSpecTexture(gAlbedoSpecTexture);

        // delegate
        shadingRenderPass(dt, shadingRenderPassShader.get());

        // display
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
    }
}
