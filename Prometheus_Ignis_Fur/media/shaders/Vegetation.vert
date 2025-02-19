#version 330 core

// vertex attributes

layout(location = 0) in vec3 aPos;         // position
layout(location = 1) in vec3 aNormal;      // normal
layout(location = 2) in vec2 aTexCoord;    // texture coord

out vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec2 WindDirection;
uniform float CurrentWindTime;
uniform float WindForce;

void main() {

    // Falta hacer que sean a destiempo
    vec2 WindEffect = (aPos.y * cos(CurrentWindTime) * WindForce) * WindDirection;

    float FinalPosX = aPos.x + WindEffect.x;
    float FinalPosZ = aPos.z + WindEffect.y;

    gl_Position = projection * view * model * vec4(FinalPosX, aPos.y, FinalPosZ, 1.0);

    texCoord = aTexCoord;

}
