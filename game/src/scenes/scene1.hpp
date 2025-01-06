#pragma once

#include "../stdafx.hpp"

#include "../core/scene.hpp"
#include "../core/shader.hpp"
#include "../core/gltfmodel.hpp"
#include "../core/windowmanager.hpp"
#include "../core/scenemanager.hpp"

namespace scene1
{
    class Scene1 : public Scene
    {
    public:
        Scene1(WindowManager* windowManager, SceneManager* sceneManager);

        ~Scene1();

        void handleKeyEvent(int key, int scancode, int action, int mods) override;

        void handleCursorPositionEvent(double x, double y) override;

        void render(float dt) override;

    private:
        std::unique_ptr<Shader> sceneShader;
        std::unique_ptr<gltfmodel::GLTFModel> sceneModel;

        // std::unique_ptr<Shader> rifleShader;
        std::unique_ptr<gltfmodel::GLTFModel> rifleModel;

        glm::vec3 cameraUp;
        glm::vec3 cameraPosition;
        glm::quat cameraOrientation;

        float fov;
    };
}// namespace scene2
