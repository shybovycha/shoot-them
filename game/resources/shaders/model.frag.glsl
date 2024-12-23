#version 420 core
#extension GL_ARB_bindless_texture : require
#extension GL_ARB_shader_storage_buffer_object : require

in VS_OUT {
    vec2 TexCoord;
    vec3 Normal;
    flat uint MaterialID;
} fs_in;

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

uniform float alpha = 1.0;

out vec4 FragColor;

void main() {
    Material material = materials[fs_in.MaterialID];

    // Sample using bindless texture handle
    vec4 baseColor = texture(sampler2D(material.baseColorTexture), fs_in.TexCoord);
    vec3 normal = texture(sampler2D(material.normalTexture), fs_in.TexCoord).rgb;

    // Apply material properties
    vec3 finalColor = baseColor.rgb * material.baseColorFactor.rgb;

    // Output final color with transition alpha
    FragColor = vec4(finalColor, baseColor.a * alpha);
}
