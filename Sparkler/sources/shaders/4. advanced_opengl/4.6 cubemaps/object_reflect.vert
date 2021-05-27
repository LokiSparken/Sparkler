#version 330 core

attribute layout (location = 0) vec3 aPosition;
attribute layout (location = 1) vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 normal;
out vec3 position;

void main()
{
    normal = mat3(transpose(inverse(model))) * aNormal;
    position = vec3(model * vec4(aPosition, 1.0f));
    gl_Position = projection * view * vec4(position, 1.0f);
}