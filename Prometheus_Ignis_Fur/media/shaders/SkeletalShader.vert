#version 330 core

// vertex attributes

layout(location = 0) in vec3 aPos;         // position
layout(location = 1) in vec3 aNormal;      // normal
layout(location = 2) in vec2 aTexCoord;    // texture coord
layout(location = 3) in ivec4 aBoneIds;    // bone ids
layout(location = 4) in vec4 aWeight;    // weight

// out vec4 ourColor;
out vec2 texCoord;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 NormalMatrix;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesTransforms[MAX_BONES];

void main() {

    vec4 totalPosition = vec4(0.0f);
    vec3 totalNormal = vec3(0.0f);
    for(int i = 0; i < MAX_BONE_INFLUENCE; i++) {
        if(aBoneIds[i] == -1)
            continue;
        if(aBoneIds[i] >= MAX_BONES) {
            totalPosition = vec4(aPos, 1.0f);
            break;
        }
        vec4 localPosition = finalBonesTransforms[aBoneIds[i]] * vec4(aPos, 1.0f);
        totalPosition += localPosition * aWeight[i];
        vec3 localNormal = mat3(finalBonesTransforms[aBoneIds[i]]) * aNormal;
        totalNormal += localNormal;
    }

    // Proceso costoso, mejor invertir en CPU y enviarlo mediante uniform y solo lo calcula una vez
    Normal = NormalMatrix * totalNormal;
    texCoord = aTexCoord;

    // FragPos = vec3(model * totalPosition);

    mat4 viewModel = view * model;

    gl_Position = projection * viewModel * totalPosition;

}