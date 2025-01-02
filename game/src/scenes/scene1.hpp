#pragma once

#include "../stdafx.hpp"

#include "../scene.hpp"
#include "../shader.hpp"
#include "../gltfmodel.hpp"
#include "../windowmanager.hpp"
#include "../scenemanager.hpp"

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
        std::unique_ptr<Shader> modelShader;
        std::unique_ptr<gltfmodel::GLTFModel> model3d;

        glm::vec3 cameraUp;
        glm::vec3 cameraPosition;
        glm::quat cameraOrientation;

        float fov;
    };
}// namespace scene2
