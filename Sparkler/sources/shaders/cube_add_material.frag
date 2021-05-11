#version 330 core

in vec3 normal;
in vec3 fragPosition;

struct Material
{
	vec3 ambientColor;
	vec3 diffuseColor;
	vec3 specularColor;
	float shininess;
};

uniform Material material;

struct Light
{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform Light light;

uniform vec3 viewPosition;

void main()
{
	float ambientCoeff = 0.1f;
	vec3 ambient = material.ambientColor * light.ambient;

	vec3 normalUnit = normalize(normal);
	vec3 lightDirection = normalize(light.position - fragPosition);
	float diffuseCoeff = max(dot(lightDirection, normalUnit), 0.0);
	vec3 diffuse = material.diffuseColor * diffuseCoeff * light.diffuse;

	vec3 viewDirection = normalize(fragPosition - viewPosition);
	vec3 reflectDirection = reflect(-viewDirection, normalUnit);
	float specularCoeff = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);
	vec3 specular = material.specularColor * specularCoeff * light.specular;

	vec3 irradiance = ambient + diffuse + specular;
	gl_FragColor = vec4(irradiance, 1.0f);
}