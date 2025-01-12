#include "gltfmodel.hpp"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_MSC_SECURE_CRT
#define TINYGLTF_USE_CPP14

#include <tiny_gltf.h>

glm::mat4 calculateLocalTransform(const tinygltf::Node& node)
{
    glm::mat4 translation {1.0f};
    glm::mat4 rotation {1.0f};
    glm::mat4 scale {1.0f};

    // Handle matrix transform if present
    if (!node.matrix.empty())
    {
        glm::mat4 matrix;
        std::copy(node.matrix.begin(), node.matrix.end(), &matrix[0][0]);
        return matrix;
    }

    // Handle TRS transforms
    if (!node.translation.empty())
    {
        translation = glm::translate(glm::mat4(1.0f), glm::vec3(node.translation[0], node.translation[1], node.translation[2]));
    }

    if (!node.rotation.empty())
    {
        glm::quat q(
                node.rotation[3],// w
                node.rotation[0],// x
                node.rotation[1],// y
                node.rotation[2] // z
        );

        rotation = glm::mat4_cast(q);
    }

    if (!node.scale.empty())
    {
        scale = glm::scale(glm::mat4(1.0f), glm::vec3(node.scale[0], node.scale[1], node.scale[2]));
    }

    return translation * rotation * scale;
}

void processNode(const tinygltf::Model& model, int nodeIndex, const glm::mat4& parentTransform, std::vector<gltfmodel::Light>& lights)
{
    const tinygltf::Node& node = model.nodes[nodeIndex];

    // Calculate node's transform
    glm::mat4 localTransform = calculateLocalTransform(node);
    glm::mat4 globalTransform = parentTransform * localTransform;

    // Check if this node has a light extension
    auto it = node.extensions.find("KHR_lights_punctual");

    if (it != node.extensions.end())
    {
        // Get light index from extension
        int lightIndex = it->second.Get("light").GetNumberAsInt();
        const tinygltf::Light& gltfLight = model.lights[lightIndex];

        if (gltfLight.type == "point")
        {
            gltfmodel::Light light;

            // Transform light position by global transform
            glm::vec4 pos = globalTransform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            light.position = glm::vec3(pos); // glm::vec3(pos) * lightSceneTransform; // but since the transform is set at a later time, can't precalculate it

            // Convert color and intensity
            light.color = glm::vec3(
                    gltfLight.color[0],
                    gltfLight.color[1],
                    gltfLight.color[2]);

            light.intensity = static_cast<float>(gltfLight.intensity);

            // Set default attenuation if not specified
            // light.attenuation = glm::vec3(1.0f, 0.09f, 0.032f);

            // Get range (radius) if specified
            light.range = gltfLight.range > 0.0f ? static_cast<float>(gltfLight.range) : 10.0f;

            lights.push_back(light);
        }
    }

    // Process child nodes
    for (int child : node.children)
    {
        processNode(model, child, globalTransform, lights);
    }
}

gltfmodel::GLTFModel::GLTFModel(std::string_view path)
{
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err, warn;

    bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, std::string(path.data()));

    if (!ret)
    {
        throw std::runtime_error("Failed to load GLTF model: " + err);
    }

    // Process vertices
    std::vector<ModelVertex> vertices;

    for (const auto& mesh : model.meshes)
    {
        for (const auto& primitive : mesh.primitives)
        {
            const auto& positionAccessor = model.accessors[primitive.attributes.at("POSITION")];
            const auto& positionBufferView = model.bufferViews[positionAccessor.bufferView];
            const auto& positionBuffer = model.buffers[positionBufferView.buffer];
            const float* positions = reinterpret_cast<const float*>(
                    &positionBuffer.data[positionBufferView.byteOffset + positionAccessor.byteOffset]);

            const auto& texcoordAccessor = model.accessors[primitive.attributes.at("TEXCOORD_0")];
            const auto& texcoordBufferView = model.bufferViews[texcoordAccessor.bufferView];
            const auto& texcoordBuffer = model.buffers[texcoordBufferView.buffer];
            const float* texcoords = reinterpret_cast<const float*>(
                    &texcoordBuffer.data[texcoordBufferView.byteOffset + texcoordAccessor.byteOffset]);

            const auto& normalAccessor = model.accessors[primitive.attributes.at("NORMAL")];
            const auto& normalBufferView = model.bufferViews[normalAccessor.bufferView];
            const auto& normalBuffer = model.buffers[normalBufferView.buffer];
            const float* normals = reinterpret_cast<const float*>(
                    &normalBuffer.data[normalBufferView.byteOffset + normalAccessor.byteOffset]);

            size_t vertexCount = positionAccessor.count;
            for (size_t i = 0; i < vertexCount; ++i)
            {
                ModelVertex vertex {
                        .position = glm::vec3(
                                positions[i * 3],
                                positions[i * 3 + 1],
                                positions[i * 3 + 2]),

                        .normal = glm::vec3(
                                normals[i * 3],
                                normals[i * 3 + 1],
                                normals[i * 3 + 2]),
                
                        .texCoord = glm::vec2(
                                        texcoords[i * 2],
                                        texcoords[i * 2 + 1])};

                vertices.push_back(vertex);
            }
        }
    }

    // Process indices
    std::vector<uint32_t> indices;

    for (const auto& mesh : model.meshes)
    {
        for (const auto& primitive : mesh.primitives)
        {
            const auto& accessor = model.accessors[primitive.indices];
            const auto& bufferView = model.bufferViews[accessor.bufferView];
            const auto& buffer = model.buffers[bufferView.buffer];

            switch (accessor.componentType)
            {
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: {
                const uint16_t* data = reinterpret_cast<const uint16_t*>(
                        &buffer.data[bufferView.byteOffset + accessor.byteOffset]);

                for (size_t i = 0; i < accessor.count; ++i)
                {
                    indices.push_back(static_cast<uint32_t>(data[i]));
                }

                break;
            }

            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT: {
                const uint32_t* data = reinterpret_cast<const uint32_t*>(
                        &buffer.data[bufferView.byteOffset + accessor.byteOffset]);

                indices.insert(indices.end(), data, data + accessor.count);

                break;
            }
            }
        }
    }

    // Create and initialize vertex buffer
    glCreateBuffers(1, &vertexBuffer);
    glNamedBufferStorage(vertexBuffer, vertices.size() * sizeof(ModelVertex),
                         vertices.data(), GL_DYNAMIC_STORAGE_BIT);

    // Create and initialize index buffer
    glCreateBuffers(1, &indexBuffer);
    glNamedBufferStorage(indexBuffer, indices.size() * sizeof(uint32_t),
                         indices.data(), GL_DYNAMIC_STORAGE_BIT);

    // Process materials
    materials.resize(model.materials.size());

    for (size_t i = 0; i < model.materials.size(); ++i)
    {
        const auto& glTFMaterial = model.materials[i];
        MaterialData& material = materials[i];

        // Base color factor
        if (glTFMaterial.pbrMetallicRoughness.baseColorFactor.size() == 4)
        {
            material.baseColorFactor = glm::vec4(
                    glTFMaterial.pbrMetallicRoughness.baseColorFactor[0],
                    glTFMaterial.pbrMetallicRoughness.baseColorFactor[1],
                    glTFMaterial.pbrMetallicRoughness.baseColorFactor[2],
                    glTFMaterial.pbrMetallicRoughness.baseColorFactor[3]);
        }
        else
        {
            material.baseColorFactor = glm::vec4(1.0f);
        }

        // Metallic and roughness factors
        material.metallicFactor = glTFMaterial.pbrMetallicRoughness.metallicFactor;
        material.roughnessFactor = glTFMaterial.pbrMetallicRoughness.roughnessFactor;

        // Load textures
        if (glTFMaterial.pbrMetallicRoughness.baseColorTexture.index >= 0)
        {
            const auto& texture = model.textures[glTFMaterial.pbrMetallicRoughness.baseColorTexture.index];
            const auto& image = model.images[texture.source];

            GLuint textureId;
            glCreateTextures(GL_TEXTURE_2D, 1, &textureId);
            glTextureStorage2D(textureId, 1, GL_RGBA8, image.width, image.height);
            glTextureSubImage2D(textureId, 0, 0, 0, image.width, image.height,
                                GL_RGBA, GL_UNSIGNED_BYTE, image.image.data());
            glTextureParameteri(textureId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTextureParameteri(textureId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // material.baseColorTexture = textureId; // no bindless textures

            material.baseColorTexture = glGetTextureHandleARB(textureId);
            glMakeTextureHandleResidentARB(material.baseColorTexture);

            textureHandles.push_back(material.baseColorTexture);
        }

        // Normal texture
        if (glTFMaterial.normalTexture.index >= 0)
        {
            const auto& texture = model.textures[glTFMaterial.normalTexture.index];
            const auto& image = model.images[texture.source];

            GLuint textureId;
            glCreateTextures(GL_TEXTURE_2D, 1, &textureId);
            glTextureStorage2D(textureId, 1, GL_RGBA8, image.width, image.height);
            glTextureSubImage2D(textureId, 0, 0, 0, image.width, image.height,
                                GL_RGBA, GL_UNSIGNED_BYTE, image.image.data());
            glTextureParameteri(textureId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTextureParameteri(textureId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // material.normalTexture = textureId; // no bindless textures

            material.normalTexture = glGetTextureHandleARB(textureId);
            glMakeTextureHandleResidentARB(material.normalTexture);

            textureHandles.push_back(material.normalTexture);
        }

        // no bindless textures
        // Store texture bindings
        // textureBindings.push_back(material.baseColorTexture);
        // textureBindings.push_back(material.normalTexture);
    }

    // Create material buffer
    glCreateBuffers(1, &materialBuffer);
    glNamedBufferStorage(materialBuffer, materials.size() * sizeof(MaterialData),
                         materials.data(), GL_DYNAMIC_STORAGE_BIT);

    // Process meshes
    size_t currentVertexOffset = 0;
    size_t currentIndexOffset = 0;

    for (const auto& mesh : model.meshes)
    {
        for (const auto& primitive : mesh.primitives)
        {
            MeshData meshData {
                .transform = glm::mat4(1.0f), // TODO: process node transforms here
                .materialIndex = (uint32_t) primitive.material,
                .vertexOffset = (uint32_t) currentVertexOffset,
                .indexOffset = (uint32_t) currentIndexOffset,
                .indexCount = (uint32_t) model.accessors[primitive.indices].count,
            };

            DrawCommand drawCommand {.count = meshData.indexCount,
                                     .instanceCount = 1,
                                     .firstIndex = (uint32_t) currentIndexOffset,
                                     .baseVertex = (int32_t) currentVertexOffset,
                                     .baseInstance = (uint32_t) meshes.size()};

            meshes.push_back(meshData);
            drawCommands.push_back(drawCommand);

            currentVertexOffset += model.accessors[primitive.attributes.at("POSITION")].count;
            currentIndexOffset += meshData.indexCount;
        }
    }

    const tinygltf::Scene& scene = model.scenes[model.defaultScene];
    
    for (int nodeIndex : scene.nodes)
    {
        processNode(model, nodeIndex, glm::mat4(1.0f), lights);
    }

    // Create mesh buffer
    glCreateBuffers(1, &meshBuffer);
    glNamedBufferStorage(meshBuffer, meshes.size() * sizeof(MeshData),
                         meshes.data(), GL_DYNAMIC_STORAGE_BIT);

    // Create VAO
    glCreateVertexArrays(1, &vertexArrayObject);

    // Position
    glEnableVertexArrayAttrib(vertexArrayObject, 0);
    glVertexArrayAttribFormat(vertexArrayObject, 0, 3, GL_FLOAT, GL_FALSE,
                              offsetof(ModelVertex, position));
    glVertexArrayAttribBinding(vertexArrayObject, 0, 0);

    // Normal
    glEnableVertexArrayAttrib(vertexArrayObject, 1);
    glVertexArrayAttribFormat(vertexArrayObject, 1, 3, GL_FLOAT, GL_FALSE,
                              offsetof(ModelVertex, normal));
    glVertexArrayAttribBinding(vertexArrayObject, 1, 0);

    // TexCoord
    glEnableVertexArrayAttrib(vertexArrayObject, 2);
    glVertexArrayAttribFormat(vertexArrayObject, 2, 2, GL_FLOAT, GL_FALSE,
                              offsetof(ModelVertex, texCoord));
    glVertexArrayAttribBinding(vertexArrayObject, 2, 0);

    glVertexArrayVertexBuffer(vertexArrayObject, 0, vertexBuffer, 0, sizeof(ModelVertex));
    
    glVertexArrayElementBuffer(vertexArrayObject, indexBuffer);

    // Create and fill draw command buffer
    glCreateBuffers(1, &drawCommandBuffer);
    glNamedBufferStorage(drawCommandBuffer,
                         drawCommands.size() * sizeof(DrawCommand),
                         drawCommands.data(),
                         GL_DYNAMIC_STORAGE_BIT);
}

gltfmodel::GLTFModel::~GLTFModel()
{
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &indexBuffer);
    glDeleteBuffers(1, &materialBuffer);
    glDeleteBuffers(1, &meshBuffer);
    glDeleteBuffers(1, &drawCommandBuffer);

    glDeleteVertexArrays(1, &vertexArrayObject);

    for (GLuint64 handle : textureHandles)
    {
        glMakeTextureHandleNonResidentARB(handle);
    }

    // glDeleteTextures(textureBindings.size(), textureBindings.data()); // no bindless textures
}

void gltfmodel::GLTFModel::render()
{
    // Bind SSBOs
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, meshBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, materialBuffer);

    glBindVertexArray(vertexArrayObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

    // Bind all textures at once using texture handles
    // glBindTextures(textureBindings[0], textureBindings.size(), textureBindings.data()); // no bindless textures

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

    // Reset bindings
    glBindVertexArray(0);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);
}

std::vector<gltfmodel::Light> gltfmodel::GLTFModel::getLights() const
{
    return lights;
}
