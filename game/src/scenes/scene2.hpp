#pragma once

#include "../stdafx.hpp"

#include "../scene.hpp"
#include "../shader.hpp"
#include "../gltfmodel.hpp"

namespace scene2
{
    class Scene2 : public Scene
    {
    public:
        Scene2();

        ~Scene2();

        void handleKeyEvent(int key, int scancode, int action, int mods);

        void handleCursorPositionEvent(GLFWwindow* w, double x, double y);

        void render(float dt);

    private:
        std::unique_ptr<Shader> modelShader;
        std::unique_ptr<gltfmodel::GLTFModel> model3d;

        glm::vec3 cameraUp;
        glm::vec3 cameraForward;
        glm::vec3 cameraPosition;
        glm::quat cameraOrientation;

        float fov;
    };
}// namespace scene2
