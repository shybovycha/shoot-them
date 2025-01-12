#include "shadingrenderpassshader.hpp"

const std::string_view VERTEX_SHADER_SOURCE = R"glsl(
#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

layout (location = 0) out vec2 TexCoords;

void main() {
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 1.0);
}
)glsl";

const std::string_view FRAGMENT_SHADER_SOURCE = R"glsl(
#version 460 core

out vec4 FragColor;

in vec2 TexCoords;

layout(binding = 0) uniform sampler2D gPosition;
layout(binding = 1) uniform sampler2D gNormal;
layout(binding = 2) uniform sampler2D gAlbedoSpec;

uniform vec3 viewPos;

struct Light {
    vec3 color;
    vec3 position;
    float intensity;
    float range;
};

const int MAX_LIGHTS = 16;
uniform Light lights[MAX_LIGHTS];
uniform int numLights;

// PBR functions
// float DistributionGGX(vec3 N, vec3 H, float roughness);
// float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
// vec3 fresnelSchlick(float cosTheta, vec3 F0);

void main() {
    // Get G-buffer values
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Albedo = texture(gAlbedoSpec, TexCoords).rgb;
    // float Metallic = texture(gAlbedoSpec, TexCoords).a;
    // float Roughness = texture(gAlbedoSpec, TexCoords).a; // Using metallic as roughness for simplicity
    
    vec3 N = normalize(Normal);
    vec3 V = normalize(viewPos - FragPos);
    
    // Calculate reflectance at normal incidence
    // Reflectance equation
    vec3 Lo = vec3(0.0);

    /*vec3 F0 = vec3(0.04);
    F0 = mix(F0, Albedo, Metallic);
    
    for(int i = 0; i < numLights; i++) {
        vec3 L = normalize(lights[i].position - FragPos);
        vec3 H = normalize(V + L);
        float distance = length(lights[i].position - FragPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = lights[i].color * lights[i].intensity * attenuation;
        
        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, Roughness);
        float G = GeometrySmith(N, V, L, Roughness);
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
        
        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;
        
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - Metallic;
        
        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * Albedo / PI + specular) * radiance * NdotL;
    }*/
    
    vec3 ambient = vec3(0.03) * Albedo;
    vec3 color = ambient + Lo;
    
    // HDR tonemapping and gamma correction
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));
    
    FragColor = vec4(color, 1.0);
}
)glsl";

deferredrendering::shaders::ShadingRenderPassShader::ShadingRenderPassShader()
    : Shader(VERTEX_SHADER_SOURCE, FRAGMENT_SHADER_SOURCE)
{
    viewPos_location = getUniformLocation("viewPos");

    positionSampler_location = getUniformLocation("gPosition");
    normalSampler_location = getUniformLocation("gNormal");
    albedoSpecSampler_location = getUniformLocation("gAlbedoSpec");

    numLights_location = getUniformLocation("numLights");
    lights_location = getUniformLocation("lights");
}

void deferredrendering::shaders::ShadingRenderPassShader::set_viewPos(glm::vec3 value) const
{
    setVec3(viewPos_location, value);
}

void deferredrendering::shaders::ShadingRenderPassShader::bindPositionTexture(GLuint textureId) const
{
    glBindSampler(textureId, positionSampler_location);
}

void deferredrendering::shaders::ShadingRenderPassShader::bindNormalTexture(GLuint textureId) const
{
    glBindSampler(textureId, normalSampler_location);
}

void deferredrendering::shaders::ShadingRenderPassShader::bindAlbedoSpecTexture(GLuint textureId) const
{
    glBindSampler(textureId, albedoSpecSampler_location);
}

void deferredrendering::shaders::ShadingRenderPassShader::set_lights(std::vector<Light> value) const
{
    setInt(numLights_location, value.size());
    // TODO: set lights[i]
}
