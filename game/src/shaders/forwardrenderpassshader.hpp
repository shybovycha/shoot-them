#pragma once

#include "../stdafx.hpp"

#include "../core/shader.hpp"

namespace deferredrendering
{
    namespace shaders
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
#ifdef _DEBUG
            GLuint baseColorTexture;
            GLuint normalTexture;
#else
            uint64_t baseColorTexture;
            uint64_t normalTexture;
#endif
            float metallicFactor;
            float roughnessFactor;
            glm::vec2 padding;
        };

        class ForwardRenderPassShader : public Shader
        {
        public:
            ForwardRenderPassShader();

            void set_modelMatrix(glm::mat4 value) const;

            void set_viewMatrix(glm::mat4 value) const;

            void set_projectionMatrix(glm::mat4 value) const;

        private:
            GLuint modelMatrix_location;
            GLuint viewMatrix_location;
            GLuint projectionMatrix_location;

            GLuint albedoMapSampler_location;
            GLuint normalMapSampler_location;
        };
    }// namespace shaders
}// namespace deferredrendering
