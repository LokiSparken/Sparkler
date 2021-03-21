#version 330 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vColor;
layout (location = 2) in vec2 vTexCoord;

uniform mat4 transform;

out vec3 color;
out vec2 texCoord;

void main()
{
    gl_Position = transform * vec4(vPos, 1.0f);
//    gl_Position = vec4(vPos, 1.0f);
    color = vColor;
    texCoord = vTexCoord;
}