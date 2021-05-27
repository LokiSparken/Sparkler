#version 330 core

in vec3 position;
in vec3 normal;

uniform vec3 cameraPosition;
uniform samplerCube skybox;

void main()
{
	float ratio = 1.00 / 1.52;
	vec3 I = normalize(position - cameraPosition);
	vec3 refractDirection = refract(I, normal, ratio);
	vec3 irradiance = vec3(texture(skybox, refractDirection));
	gl_FragColor = vec4(irradiance, 1.0f);
}