#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D depthMap;
uniform bool enable;

void main()
{
    FragColor = texture(depthMap, TexCoords);
    if(enable)
        discard;
}