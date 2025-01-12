#pragma once

#include "../stdafx.hpp"

#include "../core/shader.hpp"

namespace sceneshader
{
    struct Vertex {
        glm::vec3 position;
    };

    struct Mesh {
        glm::mat4 transform;
        unsigned int materialIndex;
        unsigned int vertexOffset;
        unsigned int indexOffset;
        unsigned int indexCount;
    };

    struct Material {
        glm::vec4 baseColorFactor;
        uint64_t baseColorTexture;
        uint64_t normalTexture;
        float metallicFactor;
        float roughnessFactor;
        glm::vec2 padding;
    };

    struct Light {
        glm::vec3 color;
        glm::vec3 position;
        float intensity;
        float range;
    };


    class SceneShader : public Shader
    {
    public:
        SceneShader();

        void set_dt(float value);

        void set_alpha(float value);

        void set_modelMatrix(glm::mat4 value);

        void set_viewMatrix(glm::mat4 value);

        void set_projectionMatrix(glm::mat4 value);

        void set_numLights(int value);

        void set_viewPos(glm::vec3 value);

        // void set_vertices_buffer(std::vector<Vertex> value);

        // void set_indices_buffer(std::vector<unsigned int> value);

        // void set_meshes_buffer(std::vector<Mesh> value);

        // void set_materials_buffer(std::vector<Material> value);

        void set_lights_buffer(std::vector<Light> value);

    private:
        GLuint dt_location;
        GLuint alpha_location;
        GLuint modelMatrix_location;
        GLuint viewMatrix_location;
        GLuint projectionMatrix_location;
        GLuint numLights_location;
        GLuint viewPos_location;
        // GLuint vertices_buffer_location;
        // GLuint indices_buffer_location;
        // GLuint meshes_buffer_location;
        // GLuint materials_buffer_location;
        GLuint lights_buffer_location;
    };
}; // namespace SceneShader
