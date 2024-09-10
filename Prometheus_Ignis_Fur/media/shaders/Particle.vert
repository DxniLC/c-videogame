#version 330 core

// vertex attributes

layout(location = 0) in vec2 aTexture;
layout(location = 1) in vec3 aPos;
layout(location = 2) in vec2 aSize;

// out vec4 ourColor;
out vec2 texCoord;

out vec3 FragPos;

uniform mat4 view;
uniform mat4 projection;

void main() {

    mat4 model = mat4(1.0);
    model[3] = vec4(aPos, 1.0);

    mat4 modelView = view * model;

    // First colunm.
    modelView[0][0] = 1.0;
    modelView[0][1] = 0.0;
    modelView[0][2] = 0.0;

    // Second Column
    modelView[1][0] = 0.0;
    modelView[1][1] = 1.0;
    modelView[1][2] = 0.0;

    // Thrid colunm.
    modelView[2][0] = 0.0;
    modelView[2][1] = 0.0;
    modelView[2][2] = 1.0;

    vec2 resizedPos = aTexture * aSize;

    vec4 position = modelView * vec4(resizedPos, 0.0, 1.0);

    gl_Position = projection * position;

    texCoord = aTexture;

}