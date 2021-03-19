#version 330 core

in vec3 color;
in vec2 texCoord;

uniform sampler2D texture1;

out vec4 fragColor;

void main()
{
	fragColor = vec4(color.xyz, 1.0f) * texture(texture1, texCoord);
}