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
    vec4 color;
    vec3 position;
    float intensity;
    float range;
};

in vec3 fragPos;
in vec2 texCoord;
in vec3 normal;
in vec3 viewDir;
in flat uint materialIndex;

layout(std430, binding = 1) readonly buffer MaterialBuffer {
    Material materials[];
};

layout(std430, binding = 2) readonly buffer LightsBuffer {
    Light lights[];
};

uniform mat4 view;
uniform float dt = 1.0;
uniform float alpha = 1.0;

layout(location = 0) out vec4 fragColor;

vec3 CalcPointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffuseColor) {
    vec3 lightDir = normalize(light.position - fragPos);
    
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 1.0); // material.metallicFactor);
    
    // attenuation
    float distance    = length(light.position - fragPos);
    float range = 1.0; // light.range
    float attenuation = max(min(1.0 - pow((distance / range), 4.0), 1 ), 0 ) / pow(distance, 2.0);
    // float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    
    // combine results
    // vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoords));
    // vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoords));
    // vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    
    // ambient  *= attenuation;
    // diffuse  *= attenuation;
    // specular *= attenuation;

    return diffuseColor * attenuation; // (ambient + diffuse + specular);
} 

void main() {
    Material material = materials[materialIndex];
    
    // Sample using bindless texture handle
    vec4 baseColor = texture(sampler2D(material.baseColorTexture), texCoord);
    vec3 normal = texture(sampler2D(material.normalTexture), texCoord).rgb;
    
    // Apply material properties
    vec3 finalColor = baseColor.rgb * material.baseColorFactor.rgb;

    vec3 viewDir = vec3(view[2][0], view[2][1], view[2][2]);

    for (int i = 0; i < lights.length(); i++) {
        finalColor *= CalcPointLight(lights[i], normal, fragPos, viewDir, finalColor);
    }
    
    // Output final color with transition alpha
    fragColor = vec4(finalColor, 1.0); //, baseColor.a * alpha);
}
