#pragma once

#include "../stdafx.hpp"

#include "../core/deferredrenderingscene.hpp"
#include "../core/gltfmodel.hpp"
#include "../core/windowmanager.hpp"
#include "../core/scenemanager.hpp"
#include "../core/scenes.hpp"
#include "../shaders/forwardrenderpassshader.hpp"
#include "../shaders/shadingrenderpassshader.hpp"

namespace scene1
{
    class Scene1 : public DeferredRenderingScene
    {
    public:
        Scene1(WindowManager* windowManager, SceneManager* sceneManager);

        ~Scene1();

        void handleKeyEvent(int key, int scancode, int action, int mods) override;

        void handleCursorPositionEvent(double x, double y) override;

        void forwardRenderPass(float dt, deferredrendering::shaders::ForwardRenderPassShader* shader) override;

        void shadingRenderPass(float dt, deferredrendering::shaders::ShadingRenderPassShader* shader) override;

    private:
        std::unique_ptr<gltfmodel::GLTFModel> sceneModel;
        std::unique_ptr<gltfmodel::GLTFModel> rifleModel;

        std::vector<deferredrendering::shaders::Light> lights;

        glm::vec3 cameraUp;
        glm::vec3 cameraPosition;
        glm::quat cameraOrientation;

        float fov;
    };
}// namespace scene1
