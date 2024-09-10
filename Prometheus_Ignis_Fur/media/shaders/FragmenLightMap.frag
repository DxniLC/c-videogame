#version 330 core

in vec2 texCoord;

// MATERIALS

struct Material {
    bool hasDiffuseTexture;
    sampler2D diffuseTexture;

    bool hasSpecularTexture;
    sampler2D specularTexture;

    bool hasLightTexture;
    sampler2D lightTexture;

    bool IsReflective;

    vec4 diffuse;
    vec4 specular;

    float shininess;

};

uniform Material material;

out vec4 fragColor; // valor de salida (color del fragmento)

void main() {

    vec4 diffuseMaterial = material.diffuse;
    if(material.hasDiffuseTexture)
        diffuseMaterial = texture(material.diffuseTexture, texCoord);

    vec4 specularMaterial = material.specular;
    if(material.hasSpecularTexture)
        specularMaterial = texture(material.specularTexture, texCoord);

    vec4 lightmapTexture = vec4(0.3, 0.3, 0.3, 1);
    if(material.hasLightTexture)
        lightmapTexture = texture(material.specularTexture, texCoord);

    fragColor = (diffuseMaterial * lightmapTexture) /* + specularMaterial */;

}