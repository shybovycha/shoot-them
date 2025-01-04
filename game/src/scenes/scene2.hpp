#pragma once

#include "../stdafx.hpp"

#include "../scene.hpp"
#include "../shader.hpp"
#include "../gltfmodel.hpp"
#include "../windowmanager.hpp"
#include "../scenemanager.hpp"

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
        std::unique_ptr<Shader> modelShader;
        std::unique_ptr<gltfmodel::GLTFModel> sceneModel;
        std::unique_ptr<gltfmodel::GLTFModel> rifleModel;

        glm::vec3 cameraUp;
        glm::vec3 cameraPosition;
        glm::quat cameraOrientation;

        float fov;
    };
}// namespace scene2
