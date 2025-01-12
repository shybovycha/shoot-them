#include "forwardrenderpassshader.hpp"

const std::string_view VERTEX_SHADER_SOURCE = R"glsl(
#version 460 core

struct Mesh {
    mat4 transform;
    uint materialIndex;
    uint vertexOffset;
    uint indexOffset;
    uint indexCount;
};

layout(std430, binding = 0) readonly buffer MeshBuffer {
    Mesh meshes[];
};

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

uniform mat4 view;
uniform mat4 projection;

uniform mat4 model;

layout (location = 0) out vec3 fragPos;
layout (location = 1) out vec2 texCoord;
layout (location = 2) out vec3 normal;
layout (location = 3) out flat uint materialIndex;
layout (location = 4) out mat4 modelTransform;

void main() {
    // Get mesh data for this instance
    Mesh mesh = meshes[gl_BaseInstance];

    modelTransform = model * mesh.transform;

    gl_Position = projection * view * modelTransform * vec4(inPosition, 1.0);
    fragPos = vec3(modelTransform * vec4(inPosition, 1.0));
    normal = mat3(transpose(inverse(modelTransform))) * inNormal;

    texCoord = inTexCoord;
    materialIndex = mesh.materialIndex;
}
)glsl";

#ifdef _DEBUG
const std::string_view FRAGMENT_SHADER_SOURCE = R"glsl(
#version 460 core

struct Material {
    vec4 baseColorFactor;
    uint baseColorTexture;
    uint normalTexture;
    float metallicFactor;
    float roughnessFactor;
    vec2 padding;
};

layout (location = 0) in vec3 fragPos;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;
layout (location = 3) in flat uint materialIndex;
layout (location = 4) in mat4 modelTransform;

layout(std430, binding = 1) readonly buffer MaterialBuffer {
    Material materials[];
};

const uint MAX_TEXTURES = 32;
layout(binding = 0) uniform sampler2D textures[MAX_TEXTURES];    // Array of base color & normal map textures

uniform mat4 view;

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

void main() {
    Material material = materials[materialIndex];

    // no bindless textures - good for using with RenderDoc
    vec4 baseColor = texture(textures[material.baseColorTexture], texCoord);
    vec3 normal = texture(textures[material.normalTexture], texCoord).rgb;

    // Apply material properties
    vec3 finalColor = baseColor.rgb * material.baseColorFactor.rgb;

    // Outputs
    gPosition = vec4(fragPos, 1.0);
    gNormal = vec4(normal, 1.0);
    gAlbedoSpec = vec4(finalColor, 1.0);
}
)glsl";
#else
const std::string_view FRAGMENT_SHADER_SOURCE = R"glsl(
#version 460

#extension GL_ARB_bindless_texture : require
#extension GL_ARB_shader_storage_buffer_object : require
#extension GL_ARB_gpu_shader_int64 : require

struct Material {
    vec4 baseColorFactor;
    uint64_t baseColorTexture;
    uint64_t normalTexture;
    float metallicFactor;
    float roughnessFactor;
    vec2 padding;
};

layout (location = 0) in vec3 fragPos;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;
layout (location = 3) in flat uint materialIndex;
layout (location = 4) in mat4 modelTransform;

layout(std430, binding = 1) readonly buffer MaterialBuffer {
    Material materials[];
};

uniform mat4 view;

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

void main() {
    Material material = materials[materialIndex];

    // Sample using bindless texture handle
    vec4 baseColor = texture(sampler2D(material.baseColorTexture), texCoord);
    vec3 normal = texture(sampler2D(material.normalTexture), texCoord).rgb;

    // Apply material properties
    vec3 finalColor = baseColor.rgb * material.baseColorFactor.rgb;

    // Outputs
    gPosition = vec4(fragPos, 1.0);
    gNormal = vec4(normal, 1.0);
    gAlbedoSpec = vec4(finalColor, 1.0);
}
)glsl";
#endif


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
