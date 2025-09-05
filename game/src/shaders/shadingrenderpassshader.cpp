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

uniform mat4 view;
uniform mat4 model;

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
    vec3 lightPos = light.position;
    vec3 lightDir = normalize(lightPos - fragPos);
    
    float distance = length(lightPos - fragPos) * 0.1;
    float attenuation = getDistanceAttenuation(distance, light.range);
    
    float intensity = light.intensity;

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.color * diff * intensity;

    // Specular
    vec3 viewDir_norm = normalize(viewDir);
    vec3 halfwayDir = normalize(lightDir + viewDir_norm);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 specular = light.color * spec * intensity;

    return (diffuse + specular) * attenuation;
}

void main() {
    // Get G-buffer values
    vec3 FragPos = (view * model * vec4(texture(gPosition, TexCoords).rgb, 1.0)).xyz;
    vec3 Normal = (view * model * vec4(texture(gNormal, TexCoords).rgb, 1.0)).xyz;
    vec3 Albedo = texture(gAlbedoSpec, TexCoords).rgb;
    
    vec3 N = normalize(Normal);
    vec3 V = normalize(viewPos - FragPos);

    vec3 Lo = vec3(0.0);
    
    for (int i = 0; i < numLights; i++) {
        // normal was not utilized
        Lo += CalcPointLight(lights[i], Albedo, N, FragPos, V); 
    }
    
    vec3 ambient = vec3(0.1) * Albedo;
    vec3 color = ambient + Lo;
    
    // HDR tonemapping
    // color = color / (color + vec3(1.0));

    // gamma correction
    color = pow(color, vec3(1.0 / 2.2));
    
    FragColor = vec4(color, 1.0);
}
)glsl";

deferredrendering::shaders::ShadingRenderPassShader::ShadingRenderPassShader()
    : Shader(VERTEX_SHADER_SOURCE, FRAGMENT_SHADER_SOURCE)
{
    modelMatrix_location = getUniformLocation("model");
    viewMatrix_location = getUniformLocation("view");

    viewPos_location = getUniformLocation("viewPos");

    positionSampler_location = getUniformLocation("gPosition");
    normalSampler_location = getUniformLocation("gNormal");
    albedoSpecSampler_location = getUniformLocation("gAlbedoSpec");

    numLights_location = getUniformLocation("numLights");
    lights_buffer_location = getSSBOLocation("LightsBuffer");

    glCreateBuffers(1, &lightsBuffer);
    /*glNamedBufferStorage(lightsBuffer, 0 * sizeof(deferredrendering::shaders::Light),
                         0, GL_DYNAMIC_STORAGE_BIT);*/
}

deferredrendering::shaders::ShadingRenderPassShader::~ShadingRenderPassShader()
{
    glDeleteBuffers(1, &lightsBuffer);
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

void deferredrendering::shaders::ShadingRenderPassShader::set_lights(std::vector<Light> value) const
{
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, lightsBuffer);

    glNamedBufferStorage(lightsBuffer, value.size() * sizeof(deferredrendering::shaders::Light),
                         value.data(), GL_DYNAMIC_STORAGE_BIT);

    // updateBufferData(lights_buffer_location, value);
    setInt(numLights_location, value.size());
}

void deferredrendering::shaders::ShadingRenderPassShader::set_modelMatrix(glm::mat4 value) const
{
    setMat4(modelMatrix_location, value);
}

void deferredrendering::shaders::ShadingRenderPassShader::set_viewMatrix(glm::mat4 value) const
{
    setMat4(viewMatrix_location, value);
}
