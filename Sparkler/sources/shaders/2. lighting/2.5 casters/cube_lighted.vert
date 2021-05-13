#version 330 core

attribute layout (location = 0) vec3 aPosition;
attribute layout (location = 1) vec3 aNormal;
attribute layout (location = 2) vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 normal;
out vec3 fragPosition;
out vec2 texCoord;

void main()
{
	normal = mat3(transpose(inverse(model))) * aNormal;
	fragPosition = vec3(model * vec4(aPosition, 1.0f));
	texCoord = aTexCoord;
	gl_Position = projection * view * vec4(fragPosition, 1.0f);
}