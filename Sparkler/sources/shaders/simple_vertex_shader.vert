#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 col;

out vec4 color;

void main()
{
    color = vec4(col.x, col.y, col.z, 1.0);
    gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);
}
