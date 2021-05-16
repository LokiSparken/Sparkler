#version 330 core

in vec2 texCoords;

uniform sampler2D textureDiffuse1;

void main()
{    
    gl_FragColor = texture(textureDiffuse1, texCoords);
}