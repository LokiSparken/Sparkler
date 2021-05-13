#version 330 core

struct Material
{
	sampler2D diffuseMap;
	sampler2D specularMap;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct PointLight
{
	vec3 position;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct DirectionalLight
{
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in vec2 textureCoords;

uniform vec3 uCameraPosition;
uniform Material uMaterial;
uniform DirectionalLight uDirectionalLight;

vec3 calculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDirection)
{
	vec3 lightDirection = normalize(light.direction);
	float receive = max(dot(normal, -lightDirection), 0.0);

	vec3 reflectDirection = reflect(lightDirection, normal);
	float ks = pow(max(dot(viewDirection, reflectDirection), 0.0), uMaterial.shininess);

	vec3 ambient = light.ambient * vec3(texture(uMaterial.diffuseMap, textureCoords));
	vec3 diffuse = light.diffuse * receive * vec3(texture(uMaterial.diffuseMap, textureCoords));
	vec3 specular = light.specular * ks * vec3(texture(uMaterial.specularMap, textureCoords));

	return ambient + diffuse + specular;
}

vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragPosition, vec3 viewDirection)
{
	// point to out(light)
	vec3 lightDirection = normalize(light.position - fragPosition);
	float receive = max(dot(normal, lightDirection), 0.0);

	vec3 reflectDirection = reflect(-lightDirection, normal);
	float ks = pow(max(dot(viewDirection, reflectDirection), 0.0), uMaterial.shininess);

	float distance = length(light.position - fragPosition);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

	vec3 ambient = light.ambient * vec3(texture(uMaterial.diffuseMap, textureCoords));
	vec3 diffuse = light.diffuse * receive * vec3(texture(uMaterial.diffuseMap, textureCoords));
	vec3 specular = light.specular * ks * vec3(texture(uMaterial.specularMap, textureCoords));

	return (ambient + diffuse + specular) * attenuation;
}

in vec3 aFragPosition;
in vec3 aNormal;

out vec4 fragColor;

void main()
{
//	vec3 normal = normalize(aNormal);
//	vec3 viewDirection = normalize(uCameraPosition - aFragPosition);
//
//	// directional light
//	vec3 irradiance = calculateDirectionalLight(uDirectionalLight, normal, viewDirection);
//
//	// point light
//
//	// fragColor = vec4(irradiance, 1.0f);
	fragColor = vec4(1.0f);
}