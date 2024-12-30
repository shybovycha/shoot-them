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

in vec3 fragPos;
in vec2 texCoord;
in vec3 normal;

uniform float dt = 1.0;

uniform float alpha = 1.0;

layout(location = 0) out vec4 fragColor;

void main() {
    fragColor = vec4(abs(fragPos), alpha);
}
