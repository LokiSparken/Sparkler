#version 330 core

in vec3 normal;
in vec3 fragPosition;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPosition;
uniform vec3 viewPosition;

void main()
{
	float ambientCoeff = 0.1f;
	vec3 ambient = ambientCoeff * lightColor;

	vec3 normalUnit = normalize(normal);
	vec3 lightDirection = normalize(lightPosition - fragPosition);
	float diffuseCoeff = max(dot(lightDirection, normalUnit), 0.0);
	vec3 diffuse = diffuseCoeff * lightColor;

	float specularIntensity = 0.5f;
	vec3 viewDirection = normalize(fragPosition - viewPosition);
	vec3 reflectDirection = reflect(-viewDirection, normalUnit);
	float shininess = 32.0;
	float specularCoeff = pow(max(dot(viewDirection, reflectDirection), 0.0), shininess);
	vec3 specular = specularIntensity * specularCoeff * lightColor;

//	vec3 irradiance = (ambient) * objectColor;
//	vec3 irradiance = (ambient + diffuse) * objectColor;
	vec3 irradiance = (ambient + diffuse + specular) * objectColor;
	gl_FragColor = vec4(irradiance, 1.0f);
}