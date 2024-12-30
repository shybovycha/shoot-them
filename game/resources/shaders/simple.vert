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

struct Vertex {
    vec3 position;
};

layout(std430, binding = 0) readonly buffer VertexBuffer {
    Vertex vertices[];
};

layout(std430, binding = 1) readonly buffer IndexBuffer {
    uint indices[];
};

layout(std430, binding = 2) readonly buffer MeshBuffer {
    Mesh meshes[];
};

uniform mat4 view;
uniform mat4 projection;

out vec3 fragPos;
out vec2 texCoord;
out vec3 normal;

void main() {
    // Get mesh data for this instance
    Mesh mesh = meshes[gl_BaseInstance];
    
    // Calculate vertex index
    uint vertexIndex = mesh.vertexOffset + indices[mesh.indexOffset + gl_VertexID];
    
    // Get vertex data from SSBO
    Vertex vertex = vertices[vertexIndex];
    
    // Transform position
    vec4 worldPos = mesh.transform * vec4(vertex.position, 1.0);
    gl_Position = projection * view * worldPos;
    
    // Pass through other vertex data
    fragPos = worldPos.xyz;
}
