#version 330 core

// vertex attributes

layout(location = 0) in vec3 aPos;         // position
layout(location = 1) in vec3 aNormal;      // normal
layout(location = 2) in vec2 aTexCoord;    // texture coord

// out vec4 ourColor;
out vec2 texCoord;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 NormalMatrix;


void main() {
    FragPos = vec3(model * vec4(aPos,1.0f));
    // Proceso costoso, mejor invertir en CPU y enviarlo mediante uniform y solo lo calcula una vez
    Normal = NormalMatrix * aNormal;  
    texCoord = aTexCoord;

    gl_Position = projection * view * vec4(FragPos, 1.0f);

}