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

void main() {
    // Get mesh data for this instance
    Mesh mesh = meshes[gl_BaseInstance];

    mat4 transform = model * mesh.transform;

    gl_Position = projection * view * transform * vec4(inPosition, 1.0);
    fragPos = vec3(transform * vec4(inPosition, 1.0));
    normal = mat3(transpose(inverse(transform))) * inNormal;

    texCoord = inTexCoord;
    materialIndex = mesh.materialIndex;
}
