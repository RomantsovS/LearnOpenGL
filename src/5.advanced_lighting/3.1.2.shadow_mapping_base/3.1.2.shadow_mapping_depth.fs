#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D depthMap;

void main()
{
    FragColor = texture(depthMap, TexCoords);
    gl_FragDepth = gl_FragCoord.z;
}