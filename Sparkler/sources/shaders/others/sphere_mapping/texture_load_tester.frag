#version 330 core

in vec2 textureCoords;

uniform sampler2D skybox;

void main()
{
    gl_FragColor = vec4(texture(skybox, textureCoords).rgb, 1.0);
}