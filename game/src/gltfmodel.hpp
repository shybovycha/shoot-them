#pragma once

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_MSC_SECURE_CRT
#define TINYGLTF_USE_CPP14

#include <tiny_gltf.h>

#include "stdafx.hpp"

#include "shader.hpp"

struct ModelVertex
{
    glm::vec3 position;
    glm::vec2 texCoord;
    glm::vec3 normal;
};

struct MaterialData
{
    glm::vec4 baseColorFactor;
    GLuint64 baseColorTexture;
    GLuint64 normalTexture;
    float metallicFactor;
    float roughnessFactor;
    glm::vec2 padding;
};

struct MeshData
{
    glm::mat4 transform;
    uint32_t materialIndex;
    uint32_t vertexOffset;
    uint32_t indexOffset;
    uint32_t indexCount;
};

class GLTFModel
{
public:
    GLTFModel(std::string_view path);
    ~GLTFModel();

    void draw(std::shared_ptr<Shader> shader);

private:
    GLuint vertexBuffer;
    GLuint indexBuffer;
    GLuint materialBuffer;
    GLuint meshBuffer;
    std::vector<GLuint64> textureHandles;
    std::vector<MeshData> meshes;
    std::vector<MaterialData> materials;
};
