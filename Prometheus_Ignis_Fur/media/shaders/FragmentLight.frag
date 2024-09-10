#version 330 core

in vec2 texCoord;

// MATERIALS

struct Material {
    bool hasDiffuseTexture;
    sampler2D diffuseTexture;

    vec4 diffuse;

};

uniform Material material;

out vec4 fragColor; // valor de salida (color del fragmento)

void main() {

    vec4 diffuseMaterial = material.diffuse;
    if(material.hasDiffuseTexture)
        diffuseMaterial = texture(material.diffuseTexture, texCoord);

    fragColor = diffuseMaterial + vec4(0.15, 0.15, 0.15, 1);

}