#version 330 core

attribute layout (location = 0) vec3 aPosition;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 texCoords;

void main()
{
	texCoords = aPosition;
	vec4 position = projection * view * model * vec4(aPosition, 1.0f);
	gl_Position = position;
}