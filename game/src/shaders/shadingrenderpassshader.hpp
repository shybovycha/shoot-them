#pragma once

#include "../stdafx.hpp"

#include "../core/shader.hpp"

namespace deferredrendering
{
    namespace shaders
    {
        struct Light {
            alignas(16) glm::vec3 color;
            alignas(16) glm::vec3 position;
            alignas(4) float intensity;
            alignas(4) float range;
        };

        class ShadingRenderPassShader : public Shader
        {
        public:
            ShadingRenderPassShader();

            void set_viewPos(glm::vec3 value) const;

            void bindPositionTexture(GLuint textureId) const;

            void bindNormalTexture(GLuint textureId) const;

            void bindAlbedoSpecTexture(GLuint textureId) const;

            void set_lights(std::vector<Light> value, GLuint buffer) const;

        private:
            GLuint viewPos_location;

            GLuint positionSampler_location;
            GLuint normalSampler_location;
            GLuint albedoSpecSampler_location;

            GLuint numLights_location;
            GLuint lights_buffer_location;
        };
    }
}
