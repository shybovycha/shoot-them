#pragma once

#include "../stdafx.hpp"

#include "../scene.hpp"
#include "../shader.hpp"

struct alignas(16) Vertex {
    glm::vec3 position;
    // glm::vec3 normal;
    // glm::vec2 texCoord;
};

struct DrawCommand {
    uint32_t count;
    uint32_t instanceCount;
    uint32_t firstIndex;
    int32_t baseVertex;
    uint32_t baseInstance;
};

struct MeshData {
    glm::mat4 transform;
    uint32_t materialIndex;
    uint32_t vertexOffset;
    uint32_t indexOffset;
    uint32_t indexCount;
};

class Scene1 : public Scene
{
public:
    Scene1();

    ~Scene1();

    void render(float dt);

private:
    std::unique_ptr<Shader> triangleShader;

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<MeshData> meshes;

    std::vector<DrawCommand> drawCommands;

    GLuint vertexArrayObject;
    GLuint vertexBuffer;
    GLuint indexBuffer;
    GLuint meshBuffer;
    GLuint drawCommandBuffer;
};
