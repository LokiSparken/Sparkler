#version 330 core

in vec3 normal;
in vec3 fragPosition;
in vec2 texCoord;

struct Material
{
	sampler2D diffuseColor;
	sampler2D specularColor;
	float shininess;
};

uniform Material material;

struct PointLight
{
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

uniform PointLight light;

uniform vec3 viewPosition;

void main()
{
	float distance = length(light.position - fragPosition);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	float ambientCoeff = 0.1f;
	vec3 ambient = vec3(texture(material.diffuseColor, texCoord)) * light.ambient * attenuation;

	vec3 normalUnit = normalize(normal);
	vec3 lightDirection = normalize(light.position - fragPosition);
	float diffuseCoeff = max(dot(-lightDirection, normalUnit), 0.0);
	vec3 diffuse = vec3(texture(material.diffuseColor, texCoord)) * diffuseCoeff * light.diffuse * attenuation;

	vec3 viewDirection = normalize(fragPosition - viewPosition);
	vec3 reflectDirection = reflect(-viewDirection, normalUnit);
	float specularCoeff = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);
	vec3 specularColor = vec3(texture(material.specularColor, texCoord));
	vec3 specular = specularColor * specularCoeff * light.specular * attenuation;

	vec3 irradiance = ambient + diffuse + specular;
	gl_FragColor = vec4(irradiance, 1.0f);
}