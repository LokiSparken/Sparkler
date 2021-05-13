#version 330 core

in vec3 irradiance;

void main()
{
	gl_FragColor = vec4(irradiance, 1.0f);
}