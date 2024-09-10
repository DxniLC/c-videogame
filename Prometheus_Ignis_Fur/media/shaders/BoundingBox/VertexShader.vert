#version 330 core

// vertex attributes

layout(location = 0) in vec3 aPos;         // position

out vec4 ourColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec4 color;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    ourColor = color;
}