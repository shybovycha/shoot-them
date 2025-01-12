#include "shadingrenderpassshader.hpp"

const std::string_view VERTEX_SHADER_SOURCE = R"glsl(
#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

layout (location = 0) out vec2 TexCoords;

void main() {
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 1.0);
}
)glsl";

const std::string_view FRAGMENT_SHADER_SOURCE = R"glsl(
#version 460 core

out vec4 FragColor;

in vec2 TexCoords;

layout(binding = 0) uniform sampler2D gPosition;
layout(binding = 1) uniform sampler2D gNormal;
layout(binding = 2) uniform sampler2D gAlbedoSpec;

uniform vec3 viewPos;

struct Light {
    vec3 color;
    vec3 position;
    float intensity;
    float range;
};

layout(std430, binding = 2) readonly buffer LightsBuffer {
    Light lights[];
};

uniform int numLights;

float getDistanceAttenuation(float distance, float radius) {
    // Normalize distance by radius
    float normalizedDist = distance / radius;
    
    // Smooth falloff to zero at radius edge
    float smoothFalloff = 1.0 - smoothstep(0.75, 1.0, normalizedDist);
    
    // Combine with inverse square falloff
    return smoothFalloff / (distance * distance + 1.0);
}

vec3 CalcPointLight(Light light, vec3 baseColor, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);

    // attenuation
    float distance    = length(light.position - fragPos);

    if (distance > light.range) {
        return vec3(0.0);
    }

    float attenuation = getDistanceAttenuation(distance, light.range);

    vec3 vec_attenuation = vec3(1.0, 0.09, 0.032);

    attenuation *= 1.0 / (vec_attenuation.x + 
                             vec_attenuation.y * distance +
                             vec_attenuation.z * distance * distance);

    float intensity = min(light.intensity, 10.0);

    // combine results
    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.color * diff * intensity;

    // Specular
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 specular = light.color * spec * intensity;

    return baseColor * diffuse * attenuation + specular * attenuation;
}

void main() {
    // Get G-buffer values
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Albedo = texture(gAlbedoSpec, TexCoords).rgb;
    
    vec3 N = normalize(Normal);
    vec3 V = normalize(viewPos - FragPos);

    vec3 Lo = vec3(0.0);
    
    for (int i = 0; i < numLights; i++) {
        Lo += CalcPointLight(lights[i], Albedo, Normal, FragPos, V); 
    }
    
    vec3 ambient = vec3(0.03) * Albedo;
    vec3 color = ambient + Lo;
    
    // HDR tonemapping and gamma correction
    // color = color / (color + vec3(1.0));
    // color = pow(color, vec3(1.0 / 2.2));
    
    FragColor = vec4(color, 1.0);
}
)glsl";

deferredrendering::shaders::ShadingRenderPassShader::ShadingRenderPassShader()
    : Shader(VERTEX_SHADER_SOURCE, FRAGMENT_SHADER_SOURCE)
{
    viewPos_location = getUniformLocation("viewPos");

    positionSampler_location = getUniformLocation("gPosition");
    normalSampler_location = getUniformLocation("gNormal");
    albedoSpecSampler_location = getUniformLocation("gAlbedoSpec");

    numLights_location = getUniformLocation("numLights");
    lights_buffer_location = getSSBOLocation("LightsBuffer");
}

void deferredrendering::shaders::ShadingRenderPassShader::set_viewPos(glm::vec3 value) const
{
    setVec3(viewPos_location, value);
}

void deferredrendering::shaders::ShadingRenderPassShader::bindPositionTexture(GLuint textureId) const
{
    glBindTextureUnit(0, textureId);
}

void deferredrendering::shaders::ShadingRenderPassShader::bindNormalTexture(GLuint textureId) const
{
    glBindTextureUnit(1, textureId);
}

void deferredrendering::shaders::ShadingRenderPassShader::bindAlbedoSpecTexture(GLuint textureId) const
{
    glBindTextureUnit(2, textureId);
}

void deferredrendering::shaders::ShadingRenderPassShader::set_lights(std::vector<Light> value, GLuint buffer) const
{
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, buffer);

    updateBufferData(lights_buffer_location, value);
    setInt(numLights_location, value.size());
}
