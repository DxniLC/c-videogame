#version 330 core

layout(location = 0) in vec2 aPosTexCoord; // la posicion tiene el atributo de posicion 0

uniform vec2 position;
uniform vec2 size;

// -10 -> 10 layers [20 layers]
uniform int layer;

uniform mat4 orthogonalProjection;

out vec2 texCoord;

void main() {

    float finalX = (aPosTexCoord.x * size.x) + position.x;
    float finalY = (aPosTexCoord.y * size.y) + position.y;

    gl_Position = orthogonalProjection * vec4(finalX, finalY, layer / 10.0f, 1.0);
    texCoord = aPosTexCoord;

}
