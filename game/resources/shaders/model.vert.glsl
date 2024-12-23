#version 420 core
#extension GL_ARB_bindless_texture : require
#extension GL_ARB_shader_storage_buffer_object : require

// Vertex attributes are now accessed through buffer addresses
layout(std430, binding = 0) buffer MaterialBuffer {
    struct Material {
        vec4 baseColorFactor;
        uint64_t baseColorTexture;
        uint64_t normalTexture;
        float metallicFactor;
        float roughnessFactor;
        vec2 padding;
    } materials[];
};

layout(std430, binding = 1) buffer MeshBuffer {
    struct Mesh {
        mat4 transform;
        uint materialIndex;
        uint vertexOffset;
        uint indexOffset;
        uint indexCount;
    } meshes[];
};

uniform mat4 view;
uniform mat4 projection;

out VS_OUT {
    vec2 TexCoord;
    vec3 Normal;
    flat uint MaterialID;
} vs_out;

void main() {
    // Get mesh data
    Mesh mesh = meshes[gl_DrawID];

    // Calculate vertex position
    vec3 position = vec3(mesh.transform * vec4(gl_VertexID + mesh.vertexOffset, 1.0));
    gl_Position = projection * view * vec4(position, 1.0);

    // Pass material ID and other data to fragment shader
    vs_out.MaterialID = mesh.materialIndex;
    vs_out.TexCoord = vec2(0.0); // Get from vertex buffer
    vs_out.Normal = vec3(0.0);   // Get from vertex buffer
}
