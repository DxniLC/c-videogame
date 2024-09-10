#version 330 core

// vertex attributes

layout(location = 0) in vec3 aPos;         // position
layout(location = 1) in vec3 aNormal;      // normal
layout(location = 2) in vec2 aTexCoord;    // texture coord
layout(location = 3) in mat4 instanceMatrix;// Instance matrix

// out vec4 ourColor;
out vec2 texCoord;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 view;
uniform mat4 projection;

void main() {
    
    FragPos = vec3(instanceMatrix * vec4(aPos, 1.0f));
    // Proceso costoso, mejor invertir en CPU y enviarlo mediante uniform y solo lo calcula una vez
    // Almacenamos un atributo para la normal y asi evitar tener que calcularlo en cada frame
    Normal = mat3(transpose(inverse(instanceMatrix))) * aNormal;
    texCoord = aTexCoord;

    gl_Position = projection * view * vec4(FragPos, 1.0f);

}