#version 330 core

in vec4 ourColor;
in vec2 texCoord;

out vec4 fragColor; // valor de salida (color del fragmento)

void main() {
    fragColor = ourColor;
}