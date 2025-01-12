#include "forwardrenderpassshader.hpp"

const std::string_view VERTEX_SHADER_SOURCE = R"glsl(
#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec4 aTangent;

layout (location = 0) out vec3 FragPos;
layout (location = 1) out vec2 TexCoords;
layout (location = 2) out mat3 TBN;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    vec4 worldPos = model * vec4(aPos, 1.0);
    FragPos = worldPos.xyz;
    TexCoords = aTexCoords;
    
    // Calculate TBN matrix for normal mapping
    vec3 N = normalize(mat3(model) * aNormal);
    vec3 T = normalize(mat3(model) * aTangent.xyz);
    vec3 B = cross(N, T) * aTangent.w;
    TBN = mat3(T, B, N);
    
    gl_Position = projection * view * worldPos;
}
)glsl";

const std::string_view FRAGMENT_SHADER_SOURCE = R"glsl(
#version 460 core

layout (location = 0) in vec3 FragPos;
layout (location = 1) in vec2 TexCoords;
layout (location = 2) in mat3 TBN;

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
// uniform sampler2D metallicRoughnessMap;

void main() {
    // Store position
    gPosition = vec4(FragPos, 1.0);
    
    // Store normal (in world space)
    vec3 normal = texture(normalMap, TexCoords).rgb * 2.0 - 1.0;
    gNormal = vec4(normalize(TBN * normal), 1.0);
    
    // Store albedo and metallic/roughness
    vec4 albedo = texture(albedoMap, TexCoords);
    // vec2 metallicRoughness = texture(metallicRoughnessMap, TexCoords).bg;
    
    gAlbedoSpec = albedo; // vec4(albedo.rgb, metallicRoughness.r); // Store metallic in alpha
}
)glsl";

deferredrendering::shaders::ForwardRenderPassShader::ForwardRenderPassShader()
    : Shader(VERTEX_SHADER_SOURCE, FRAGMENT_SHADER_SOURCE)
{
    modelMatrix_location = getUniformLocation("model");
    viewMatrix_location = getUniformLocation("view");
    projectionMatrix_location = getUniformLocation("projection");

    albedoMapSampler_location = getUniformLocation("albedoMap");
    normalMapSampler_location = getUniformLocation("normalMap");
}

void deferredrendering::shaders::ForwardRenderPassShader::set_modelMatrix(glm::mat4 value) const
{
    setMat4(modelMatrix_location, value);
}

void deferredrendering::shaders::ForwardRenderPassShader::set_viewMatrix(glm::mat4 value) const
{
    setMat4(viewMatrix_location, value);
}

void deferredrendering::shaders::ForwardRenderPassShader::set_projectionMatrix(glm::mat4 value) const
{
    setMat4(projectionMatrix_location, value);
}

void deferredrendering::shaders::ForwardRenderPassShader::bindAlbedoMapTexture(GLuint textureId) const
{
    glBindSampler(albedoMapSampler_location, textureId);
}

void deferredrendering::shaders::ForwardRenderPassShader::bindNormalMapTexture(GLuint textureId) const
{
    glBindSampler(normalMapSampler_location, textureId);
}
