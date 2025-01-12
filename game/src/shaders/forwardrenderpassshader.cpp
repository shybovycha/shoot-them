#include "forwardrenderpassshader.hpp"

const std::string_view NEW_VERTEX_SHADER_CODE = R"glsl(
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

const std::string_view NEW_FRAGMENT_SHADER_SOURCE = R"glsl(
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

const uint MAX_TEXTURES = 16;

layout(binding = 0) uniform sampler2D textures[MAX_TEXTURES];    // Array of base color & normal map textures

uniform mat4 view;

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

void main() {
    Material material = materials[materialIndex];
    
    // Sample using bindless texture handle
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
