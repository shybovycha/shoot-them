#pragma once

#include "../stdafx.hpp"

#include "shader.hpp"

namespace gltfmodel
{
    struct alignas(16) ModelVertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoord;
    };

    struct alignas(16) MaterialData {
        glm::vec4 baseColorFactor;
        GLuint baseColorTexture;
        GLuint normalTexture;
        float metallicFactor;
        float roughnessFactor;
        glm::vec2 padding;
    };

    struct alignas(16) MeshData {
        glm::mat4 transform;
        uint32_t materialIndex;
        uint32_t vertexOffset;
        uint32_t indexOffset;
        uint32_t indexCount;
    };

    struct DrawCommand {
        uint32_t count;
        uint32_t instanceCount;
        uint32_t firstIndex;
        int32_t baseVertex;
        uint32_t baseInstance;
    };

    struct alignas(16) Light {
        alignas(16) glm::vec3 color;
        alignas(16) glm::vec3 position;
        alignas(4) float intensity;
        alignas(4) float range;
    };

    class GLTFModel
    {
    public:
        GLTFModel(std::string_view path);

        ~GLTFModel();

        // void render(std::shared_ptr<Shader> shader);
        void render();

        std::vector<Light> getLights() const;

    private:
        GLuint vertexBuffer;
        GLuint indexBuffer;
        GLuint materialBuffer;
        GLuint meshBuffer;
        //GLuint lightsBuffer;

        GLuint drawCommandBuffer;

        GLuint vertexArrayObject;

        std::vector<GLuint> textureBindings;
        std::vector<MeshData> meshes;
        std::vector<MaterialData> materials;
        std::vector<Light> lights;
        std::vector<DrawCommand> drawCommands;
    };
}// namespace gltfmodel
