#version 330 core

uniform float uKd;
uniform vec3 uAmbient;

void main()
{
	vec3 ambient = vec3(65.0/255.0, 105.0/255.0, 225.0/255.0);
	vec3 diffuse = uKd * ambient;
	vec3 specular;
	gl_FragColor = vec4(ambient + diffuse, 1.0f);
}