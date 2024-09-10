#version 330 core

layout(location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>

out vec2 TexCoords;

uniform mat4 orthogonalProjection;
uniform int layer;

void main() {
    gl_Position = orthogonalProjection * vec4(vertex.xy, layer / 10.0f, 1.0);
    TexCoords = vertex.zw;
}