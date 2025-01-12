#pragma once

#include "../stdafx.hpp"

#include "./scene.hpp"
#include "./scenemanager.hpp"
#include "./windowmanager.hpp"

#include "../shaders/forwardrenderpassshader.hpp"
#include "../shaders/shadingrenderpassshader.hpp"

class DeferredRenderingScene : public Scene
{
public:
    DeferredRenderingScene(WindowManager* windowManager, SceneManager* sceneManager);

    ~DeferredRenderingScene();
    
    virtual void render(float dt) override;

    virtual void forwardRenderPass(float dt, deferredrendering::shaders::ForwardRenderPassShader* shader) = 0;
    
    virtual void shadingRenderPass(float dt, deferredrendering::shaders::ShadingRenderPassShader* shader) = 0;

private:
    std::unique_ptr<deferredrendering::shaders::ForwardRenderPassShader> forwardRenderPassShader;
    std::unique_ptr<deferredrendering::shaders::ShadingRenderPassShader> shadingRenderPassShader;

    // deferred rendering
    GLuint gPositionTexture;
    GLuint gNormalTexture;
    GLuint gAlbedoSpecTexture;
    GLuint gBuffer;
    GLuint rboDepthBuffer;

    GLuint quadVAO;
    GLuint quadVBO;
};
