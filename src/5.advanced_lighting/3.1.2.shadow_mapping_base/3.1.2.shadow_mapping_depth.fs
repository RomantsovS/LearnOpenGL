#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D diffuseTexture;

void main()
{             
    gl_FragColor = texture(diffuseTexture, TexCoords);
}