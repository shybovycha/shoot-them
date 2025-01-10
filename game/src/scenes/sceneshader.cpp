#include "sceneshader.hpp"

const std::string_view VERTEX_SOURCE = R"glsl(
#version 460

#extension GL_ARB_bindless_texture : require
#extension GL_ARB_shader_storage_buffer_object : require

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

out vec3 fragPos;
out vec2 texCoord;
out vec3 normal;
out flat uint materialIndex;
out mat4 modelTransform;

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

const std::string_view FRAGMENT_SOURCE = R"glsl(
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

struct Light {
    vec3 color;
    vec3 position;
    float intensity;
    float range;
};

in vec3 fragPos;
in vec2 texCoord;
in vec3 normal;
in vec3 viewDir;
in flat uint materialIndex;
in mat4 modelTransform;

layout(std430, binding = 1) readonly buffer MaterialBuffer {
    Material materials[];
};

layout(std430, binding = 2) readonly buffer LightsBuffer {
    Light lights[];
};

uniform mat4 view;
uniform float dt = 1.0;
uniform float alpha = 1.0;
uniform int numLights = 0;
uniform vec3 viewPos;

layout(location = 0) out vec4 fragColor;

float getDistanceAttenuation(float distance, float radius) {
    // Normalize distance by radius
    float normalizedDist = distance / radius;
    
    // Smooth falloff to zero at radius edge
    float smoothFalloff = 1.0 - smoothstep(0.75, 1.0, normalizedDist);
    
    // Combine with inverse square falloff
    return smoothFalloff / (distance * distance + 1.0);
}

void main() {
    Material material = materials[materialIndex];
    
    // Sample using bindless texture handle
    vec4 baseColor = texture(sampler2D(material.baseColorTexture), texCoord);
    vec3 normal = texture(sampler2D(material.normalTexture), texCoord).rgb;
    
    // Apply material properties
    // vec3 baseColor = baseColor.rgb * material.baseColorFactor.rgb;
    vec3 finalColor = vec3(0.0);

    vec3 col1 = baseColor.rgb * material.baseColorFactor.rgb;

    // vec3 viewDir = vec3(view[2][0], view[2][1], view[2][2]);
    vec3 viewDir = normalize(viewPos - fragPos);

    for (int i = 0; i < numLights; i++) {
        // finalColor += CalcPointLight(lights[i], normal, fragPos, viewDir, baseColor.rgb);
        Light light = lights[i];
        
        vec4 p1 = modelTransform * vec4(light.position, 1.0);
        vec3 lightPos = p1.xyz; // light.position; // p1.xyz;

        float lightIntensity = 20.1; // light.intensity;
        float lightRadius = 5.1; // light.range;

        vec3 lightDir = normalize(lightPos - fragPos);

        float distance = length(lightPos - fragPos);
        vec3 lightAttenuation = vec3(1.0f, 0.09f, 0.032f); // constant, linear, quadratic

        float attenuation = getDistanceAttenuation(distance, lightRadius);

        attenuation *= 1.0 / (lightAttenuation.x + 
                              lightAttenuation.y * distance +
                              lightAttenuation.z * distance * distance);

        float diff = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = light.color * diff * lightIntensity;

        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
        vec3 specular = light.color * spec * lightIntensity;

        finalColor += (col1 * diffuse * attenuation) + (specular * attenuation);
    }

    finalColor += col1 * 0.1; // ambient

    // Output final color with transition alpha
    fragColor = vec4(finalColor, 1.0); //, baseColor.a * alpha);
}
)glsl";

sceneshader::SceneShader::SceneShader()
    : Shader(VERTEX_SOURCE, FRAGMENT_SOURCE)
{
     dt_location = getUniformLocation("dt");
    alpha_location = getUniformLocation("alpha");
    modelMatrix_location = getUniformLocation("model");
    viewMatrix_location = getUniformLocation("view");
    projectionMatrix_location = getUniformLocation("projection");
    numLights_location = getUniformLocation("numLights");
    viewPos_location = getUniformLocation("viewPos");
    // vertices_buffer_location = getSSBOLocation(program, "vertices");
    // indices_buffer_location = getSSBOLocation(program, "indices");
    // meshes_buffer_location = getSSBOLocation(program, "meshes");
    // materials_buffer_location = getSSBOLocation(program, "materials");
    lights_buffer_location = getSSBOLocation("LightsBuffer");
}

void sceneshader::SceneShader::set_dt(float value)
{
    setFloat(dt_location, value);
}

void sceneshader::SceneShader::set_alpha(float value)
{
    setFloat(alpha_location, value);
}

void sceneshader::SceneShader::set_modelMatrix(glm::mat4 value)
{
    setMat4(modelMatrix_location, value);
}

void sceneshader::SceneShader::set_viewMatrix(glm::mat4 value)
{
    setMat4(viewMatrix_location, value);
}

void sceneshader::SceneShader::set_projectionMatrix(glm::mat4 value)
{
    setMat4(projectionMatrix_location, value);
}

void sceneshader::SceneShader::set_numLights(int value)
{
    setInt(numLights_location, value);
}

void sceneshader::SceneShader::set_viewPos(glm::vec3 value)
{
    setVec3(viewPos_location, value);
}

// void sceneshader::SceneShader::set_vertices_buffer(std::vector<Vertex> value)
// {
//         updateBufferData(vertices_buffer_location, value);
// }

// void sceneshader::SceneShader::set_indices_buffer(std::vector<unsigned int> value)
// {
//         updateBufferData(indices_buffer_location, value);
// }

// void sceneshader::SceneShader::set_meshes_buffer(std::vector<Mesh> value)
// {
//         updateBufferData(meshes_buffer_location, value);
// }

// void sceneshader::SceneShader::set_materials_buffer(std::vector<Material> value)
// {
//         updateBufferData(materials_buffer_location, value);
// }

void sceneshader::SceneShader::set_lights_buffer(std::vector<Light> value)
{
    updateBufferData(lights_buffer_location, value);
    setInt(numLights_location, value.size());
}
