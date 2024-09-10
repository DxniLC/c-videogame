#version 330 core

in vec2 texCoord;
in vec3 Normal;
in vec3 FragPos;

// MATERIALS

struct Material {
    bool hasDiffuseTexture;
    sampler2D diffuseTexture;

    bool hasSpecularTexture;
    sampler2D specularTexture;

    bool IsReflective;

    vec4 diffuse;
    vec4 specular;
    float shininess;

};

vec4 diffuseMaterial, specularMaterial;

uniform Material material;

// LIGHTS 
// PointLight, DirectionalLight, SpotLight

#define MAX_POINT_LIGHTS 4
#define MAX_SPOT_LIGHTS 4

struct PointLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;

    bool HasAttenuation;

};

struct DirectionalLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    bool Lighted;

};

struct SpotLight {
    vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    bool HasAttenuation;

};

uniform DirectionalLight dirLight;

uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int sizePointsLights;

uniform SpotLight spotLights[MAX_SPOT_LIGHTS];
uniform int sizeSpotLights;

// TOON SHADER
#define TOON_COLOR_LEVEL 3
#define TOON_SCALE_FACTOR 1.0f/TOON_COLOR_LEVEL

uniform vec3 viewPos;
uniform vec3 viewDirection;

out vec4 fragColor; // valor de salida (color del fragmento)

// Filter Color
uniform bool HasFilter;
uniform vec4 colorFilter;

vec3 getDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 getPointLight(PointLight light, vec3 normal, vec3 viewDir);
vec3 getSpotLight(SpotLight light, vec3 normal, vec3 viewDir);

// Toon Shader
float getToonShaderDiffuse(float diffuse);

void main() {

    diffuseMaterial = material.diffuse;
    specularMaterial = material.specular;

    if(material.hasDiffuseTexture) {
        diffuseMaterial = texture(material.diffuseTexture, texCoord);
    }

    if(material.hasSpecularTexture) {
        specularMaterial = texture(material.specularTexture, texCoord);
    }

    // Properties
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 LightOutput = vec3(0, 0, 0);

    // Directiona Light. (SUN)
    if(dirLight.Lighted)
        LightOutput = getDirectionalLight(dirLight, normal, viewDir);

    // Point Lights
    for(int i = 0; i < sizePointsLights; i++) {
        LightOutput += getPointLight(pointLights[i], normal, viewDir);
    }

    // Spot Lights
    for(int i = 0; i < sizeSpotLights; i++) {
        LightOutput += getSpotLight(spotLights[i], normal, viewDir);
    }

    if(LightOutput == vec3(0, 0, 0))
        LightOutput = diffuseMaterial.rgb * vec3(0.3, 0.3, 0.3);

    if(HasFilter) {
        fragColor = mix(vec4(LightOutput, 1.0f), vec4(colorFilter.rgb, 1), colorFilter.a);
    } else {
        fragColor = vec4(LightOutput, 1.0f);
    }

}

vec3 getDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir) {

    vec3 ambient = vec3(0, 0, 0);
    vec3 diffuse = vec3(0, 0, 0);
    vec3 specular = vec3(0, 0, 0);

    // Ambient
    ambient = light.ambient * diffuseMaterial.rgb;

    // Diffuse
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    if(diff > 0) {
        diff = getToonShaderDiffuse(diff);
    }
    diffuse = light.diffuse * diff * diffuseMaterial.rgb;

    // Specular
    if(material.IsReflective) {
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        specular = light.specular * spec * specularMaterial.rgb;
    }

    return (ambient + diffuse + specular);
}

vec3 getPointLight(PointLight light, vec3 normal, vec3 viewDir) {

    vec3 ambient = vec3(0, 0, 0);
    vec3 diffuse = vec3(0, 0, 0);
    vec3 specular = vec3(0, 0, 0);

    // Ambient
    ambient = light.ambient * diffuseMaterial.rgb;

    // Diffuse
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(normal, lightDir), 0.0);

    if(diff > 0) {
        diff = getToonShaderDiffuse(diff);
    }

    diffuse = light.diffuse * diff * diffuseMaterial.rgb;

    // Specular
    if(material.IsReflective) {
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        specular = light.specular * spec * specularMaterial.rgb;
    }

    // Attenuation
    if(light.HasAttenuation) {
        float distance = length(light.position - FragPos);
        float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
        ambient *= attenuation;
        diffuse *= attenuation;
        specular *= attenuation;
    }

    return (ambient + diffuse + specular);

}

vec3 getSpotLight(SpotLight light, vec3 normal, vec3 viewDir) {

    vec3 ambient = vec3(0, 0, 0);
    vec3 diffuse = vec3(0, 0, 0);
    vec3 specular = vec3(0, 0, 0);

    // Ambient
    ambient = light.ambient * diffuseMaterial.rgb;

    // Diffuse
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(normal, lightDir), 0.0);

    if(diff > 0) {
        diff = getToonShaderDiffuse(diff);
    }

    diffuse = light.diffuse * diff * diffuseMaterial.rgb;

    // Specular
    if(material.IsReflective) {
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        specular = light.specular * spec * specularMaterial.rgb;
    }

    if(light.HasAttenuation) {
        // Attenuation
        float distance = length(light.position - FragPos);
        float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

        ambient *= attenuation;
        diffuse *= attenuation;
        specular *= attenuation;

    }

    // SpotLight intensity
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0f, 1.0f);

    ambient *= intensity;
    diffuse *= intensity;
    specular *= intensity;

    return (ambient + diffuse + specular);

}

float getToonShaderDiffuse(float diffuse) {
    return ceil(diffuse * TOON_COLOR_LEVEL) * TOON_SCALE_FACTOR; // Ceil Redonde arriba y floor hacia abajo. Si queremos que brille menos o mas, cambiar
}