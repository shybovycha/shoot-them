#pragma once

#include "../stdafx.hpp"

#include "../core/scene.hpp"
#include "../core/gltfmodel.hpp"
#include "../core/windowmanager.hpp"
#include "../core/scenemanager.hpp"
#include "../shaders/forwardrenderpassshader.hpp"
#include "../shaders/shadingrenderpassshader.hpp"

namespace scene2
{
    class Scene2 : public Scene
    {
    public:
        Scene2(WindowManager* windowManager, SceneManager* sceneManager);

        ~Scene2();

        void handleKeyEvent(int key, int scancode, int action, int mods) override;

        void handleCursorPositionEvent(double x, double y) override;

        void render(float dt) override;

    private:
        std::unique_ptr<deferredrendering::shaders::ForwardRenderPassShader> forwardRenderPassShader;
        std::unique_ptr<deferredrendering::shaders::ShadingRenderPassShader> shadingRenderPassShader;

        std::unique_ptr<gltfmodel::GLTFModel> sceneModel;
        std::unique_ptr<gltfmodel::GLTFModel> rifleModel;
        
        std::vector<deferredrendering::shaders::Light> lights;

        glm::vec3 cameraUp;
        glm::vec3 cameraPosition;
        glm::quat cameraOrientation;

        float fov;

        // deferred rendering
        GLuint gPositionTexture;
        GLuint gNormalTexture;
        GLuint gAlbedoSpecTexture;
        GLuint gBuffer;
        GLuint rboDepthBuffer;

        GLuint lightsBuffer;

        GLuint quadVAO;
        GLuint quadVBO;
    };
}// namespace scene2
