#version 330 core

attribute layout (location = 0) vec3 aPosition;
attribute layout (location = 1) vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPosition;
uniform vec3 viewPosition;

out vec3 irradiance;

void main()
{
	float ambientCoeff = 0.1f;
	vec3 ambient = ambientCoeff * lightColor;

	vec3 vertexPosition = mat3(model) * aPosition;
	vec3 normal = mat3(transpose(inverse(model))) * aNormal;
	vec3 normalUnit = normalize(normal);
	vec3 lightDirection = normalize(lightPosition - vertexPosition);
	float diffuseCoeff = max(dot(lightDirection, normalUnit), 0.0);
	vec3 diffuse = diffuseCoeff * lightColor;

	float specularIntensity = 2.0f;
	vec3 viewDirection = normalize(vertexPosition - viewPosition);
	vec3 reflectDirection = reflect(-viewDirection, normalUnit);
	float shininess = 32.0;
	float specularCoeff = pow(max(dot(viewDirection, reflectDirection), 0.0), shininess);
	vec3 specular = specularIntensity * specularCoeff * lightColor;

	irradiance = (ambient + diffuse + specular) * objectColor;
	gl_Position = projection * view * model * vec4(aPosition, 1.0f);
}