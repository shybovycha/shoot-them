#include "scene1.hpp"

scene1::Scene1::Scene1(WindowManager* windowManager)
    : windowManager(windowManager)
{
    triangleShader = std::make_unique<Shader>("resources/shaders/simple.vert", "resources/shaders/simple.frag");

    vertices.push_back({.position = {0.0f, 0.5f, 0.0f}});
    vertices.push_back({.position = {0.5f, -0.5f, 0.0f}});
    vertices.push_back({.position = {-0.5f, -0.5f, 0.0f}});

    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);

    glCreateBuffers(1, &vertexBuffer);
    glNamedBufferStorage(vertexBuffer, vertices.size() * sizeof(Vertex),
                         vertices.data(), GL_DYNAMIC_STORAGE_BIT);

    // Create and initialize index buffer
    glCreateBuffers(1, &indexBuffer);
    glNamedBufferStorage(indexBuffer, indices.size() * sizeof(uint32_t),
                         indices.data(), GL_DYNAMIC_STORAGE_BIT);

    // Set up vertex attributes
    glCreateVertexArrays(1, &vertexArrayObject);
    glVertexArrayVertexBuffer(vertexArrayObject, 0, vertexBuffer, 0, sizeof(glm::vec3));

    // Position
    glEnableVertexArrayAttrib(vertexArrayObject, 0);
    glVertexArrayAttribFormat(vertexArrayObject, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
    glVertexArrayAttribBinding(vertexArrayObject, 0, 0);

    meshes.push_back(
            MeshData{.transform = glm::mat4(1.0f),
             .materialIndex = 0,
             .vertexOffset = 0,
             .indexOffset = 0,
             .indexCount = (unsigned int) indices.size()});

    drawCommands.push_back(DrawCommand { .count = (unsigned int) indices.size(),
    .instanceCount = 1,
    .firstIndex = 0,
    .baseVertex = 0,
    .baseInstance = 0});

    // Create mesh buffer
    glCreateBuffers(1, &meshBuffer);
    glNamedBufferStorage(meshBuffer, meshes.size() * sizeof(MeshData),
                         meshes.data(), GL_DYNAMIC_STORAGE_BIT);

    // Create and fill draw command buffer
    glCreateBuffers(1, &drawCommandBuffer);
    glNamedBufferStorage(drawCommandBuffer,
                         drawCommands.size() * sizeof(DrawCommand),
                         drawCommands.data(),
                         GL_DYNAMIC_STORAGE_BIT);
}

scene1::Scene1::~Scene1()
{
    std::cout << "cleaning up scene1" << std::endl;

    glDeleteBuffers(1, &drawCommandBuffer);
    glDeleteBuffers(1, &meshBuffer);
    glDeleteBuffers(1, &indexBuffer);
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteVertexArrays(1, &vertexArrayObject);
}

void scene1::Scene1::handleKeyEvent(int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        std::cout << "[scene1] ESC pressed" << std::endl;
    }
}

void scene1::Scene1::render(float dt)
{
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));

    glm::vec2 windowSize = windowManager->getWindowSize();
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float) windowSize.x / (float) windowSize.y, 0.1f, 100.0f);

    triangleShader->setFloat("dt", dt);
    triangleShader->setMat4("model", model);
    triangleShader->setMat4("view", view);
    triangleShader->setMat4("projection", projection);
    
    triangleShader->use();

    // Bind SSBOs
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vertexBuffer);  // Vertices
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, indexBuffer);   // Indices
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, meshBuffer);    // Mesh info
    // glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, materialBuffer);// Materials

    // Bind VAO
    glBindVertexArray(vertexArrayObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

    // Bind indirect command buffer
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, drawCommandBuffer);

    // Draw all meshes with a single draw call
    glMultiDrawElementsIndirect(
            GL_TRIANGLES,
            GL_UNSIGNED_INT,
            nullptr,            // Offset into command buffer
            drawCommands.size(),// Number of commands
            0                   // Stride (0 means tightly packed)
    );
}
