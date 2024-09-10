#version 330 core

uniform sampler2D HUDTexture;

uniform bool HasRect;
uniform bool MixColor;
uniform vec4 color;

in vec2 texCoord;

out vec4 fragColor;

void main() {

    if(HasRect)
        fragColor = color;
    else if(MixColor)
        fragColor = mix(texture(HUDTexture, texCoord), color, color.a);
    else
        fragColor = texture(HUDTexture, texCoord);

}