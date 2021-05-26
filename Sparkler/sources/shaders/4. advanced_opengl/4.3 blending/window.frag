#version 330 core

in vec2 texCoords;

uniform sampler2D texture1;

void main()
{
	gl_FragColor = texture(texture1, texCoords);
}