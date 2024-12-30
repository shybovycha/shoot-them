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
// in flat uint MaterialID;

//layout(std430, binding = 0) readonly buffer MaterialBuffer {
//    Material materials[];
//};

uniform float dt = 1.0;

uniform float alpha = 1.0;

layout(location = 0) out vec4 fragColor;

void main() {
    // Material material = materials[MaterialID];
    
    // Sample using bindless texture handle
    // vec4 baseColor = texture(sampler2D(material.baseColorTexture), TexCoord);
    // vec3 normal = texture(sampler2D(material.normalTexture), TexCoord).rgb;
    
    // Apply material properties
    // vec3 finalColor = baseColor.rgb * material.baseColorFactor.rgb;
    
    // Output final color with transition alpha
    // fragColor = vec4(finalColor, baseColor.a * alpha);
    fragColor = vec4(fragPos * dt, alpha);
}
