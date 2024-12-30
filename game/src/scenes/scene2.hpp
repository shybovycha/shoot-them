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

        void render(float dt);

    private:
        std::unique_ptr<Shader> modelShader;
        std::unique_ptr<gltfmodel::GLTFModel> model3d;
    };
}// namespace scene2