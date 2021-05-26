#version 330 core

in vec2 texCoords;

uniform sampler2D texture1;

void main()
{
	vec4 rgba = texture(texture1, texCoords);
	if (rgba.a < 0.1)
		discard;
	gl_FragColor = texture(texture1, texCoords);
}