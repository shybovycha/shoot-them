#pragma once

#include "../stdafx.hpp"

#include "../core/scene.hpp"
#include "../core/gltfmodel.hpp"
#include "../core/windowmanager.hpp"
#include "../core/scenemanager.hpp"
#include "sceneshader.hpp"

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
        std::unique_ptr<sceneshader::SceneShader> modelShader;
        std::unique_ptr<gltfmodel::GLTFModel> sceneModel;
        std::unique_ptr<gltfmodel::GLTFModel> rifleModel;
        
        std::vector<gltfmodel::Light> lights;

        glm::vec3 cameraUp;
        glm::vec3 cameraPosition;
        glm::quat cameraOrientation;

        float fov;
    };
}// namespace scene2
