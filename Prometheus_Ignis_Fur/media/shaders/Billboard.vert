#version 330 core

// vertex attributes

layout(location = 0) in vec2 aPos_Texture;         // position

// out vec4 ourColor;
out vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 viewPos;
uniform vec3 viewDirection;

uniform vec3 BillboardLockAxis;
uniform vec2 BillboardSize;

void main() {

    mat4 modelView = view * model;

    // First colunm.
    if(BillboardLockAxis.x == 0) {
        modelView[0][0] = 1.0;
        modelView[0][1] = 0.0;
        modelView[0][2] = 0.0;
    }

    // Second Column
    if(BillboardLockAxis.y == 0) {
        modelView[1][0] = 0.0;
        modelView[1][1] = 1.0;
        modelView[1][2] = 0.0;
    }

    // Thrid colunm.
    if(BillboardLockAxis.z == 0) {
        modelView[2][0] = 0.0;
        modelView[2][1] = 0.0;
        modelView[2][2] = 1.0;
    }

    vec2 offset = vec2(BillboardSize.x / 2.0, BillboardSize.y / 2.0);

    vec2 resizedPos = (aPos_Texture * BillboardSize) - offset;

    vec4 position = modelView * vec4(resizedPos, 0.0, 1.0);

    gl_Position = projection * position;

    texCoord = vec2(aPos_Texture.x,-aPos_Texture.y);

}
