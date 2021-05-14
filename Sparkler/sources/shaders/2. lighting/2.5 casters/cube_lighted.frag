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

struct DirectionalLight
{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

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

struct SpotLight
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	vec3 position;
	vec3 direction;
	float cutoff;
	float outerCutoff;

	float constant;
	float linear;
	float quadratic;
};

uniform vec3 viewPosition;
uniform Material material;
uniform DirectionalLight directionalLight;
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;

vec3 calculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDirection)
{
	vec3 lightDirection = normalize(-light.direction);

	vec3 ambient = light.ambient * texture(material.diffuseColor, texCoord).rgb;

	float diffuseCoeff = max(dot(-lightDirection, normal), 0.0f);
	vec3 diffuse = light.diffuse * diffuseCoeff * texture(material.diffuseColor, texCoord).rgb;

	vec3 reflectDirection = reflect(-viewDirection, normal);
	float specularCoeff = pow(max(dot(viewDirection, reflectDirection), 0.0f), material.shininess);
	vec3 specular = light.specular * specularCoeff * texture(material.specularColor, texCoord).rgb;

	return ambient + diffuse + specular;
//	return ambient + diffuse;
}

vec3 calculatePointLight(PointLight light, vec3 normal, vec3 viewDirection)
{
	vec3 lightDirection = normalize(fragPosition - light.position);
	float distance = length(fragPosition - light.position);
	float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * distance * distance);

	vec3 ambient = light.ambient * texture(material.diffuseColor, texCoord).rgb;

	float diffuseCoeff = max(dot(lightDirection, normal), 0.0f);
	vec3 diffuse = light.diffuse * diffuseCoeff * texture(material.diffuseColor, texCoord).rgb;

	vec3 reflectDirection = reflect(-viewDirection, normal);
	float specularCoeff = pow(max(dot(viewDirection, reflectDirection), 0.0f), material.shininess);
	vec3 specular = light.specular * specularCoeff * texture(material.specularColor, texCoord).rgb;

	return ambient + (diffuse + specular) * attenuation;
}

vec3 calculateSpotLight(SpotLight light, vec3 normal, vec3 viewDirection)
{
	vec3 tracingDirection = vec3(fragPosition - light.position);
	float cos_theta   = dot(normalize(tracingDirection), normalize(light.direction));
	float epsilon     = light.cutoff - light.outerCutoff;
	float attenuation = clamp((cos_theta - light.outerCutoff) / epsilon, 0.0f, 1.0f);

	float distance = length(fragPosition - light.position);
	float distanceAttenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * distance * distance);

	float ambientCoeff = 0.1f;
	vec3 ambient = light.ambient * texture(material.diffuseColor, texCoord).rgb;

	vec3 lightDirection = normalize(light.position - fragPosition);
	float diffuseCoeff = max(dot(lightDirection, normal), 0.0);
	vec3 diffuse = light.diffuse * diffuseCoeff * texture(material.diffuseColor, texCoord).rgb;

	vec3 reflectDirection = reflect(-viewDirection, normal);
	float specularCoeff = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);
	vec3 specularColor = texture(material.specularColor, texCoord).rgb;
	vec3 specular = light.specular * specularCoeff * specularColor;
	
	return ambient + (diffuse + specular) * attenuation;
}

void main()
{
	vec3 normalUnit = normalize(normal);
	vec3 viewDirection = normalize(vec3(fragPosition - viewPosition));
	
	vec3 irradiance = calculateDirectionalLight(directionalLight, normalUnit, viewDirection);
	for (int i=0; i<NR_POINT_LIGHTS; ++i)
	{
		irradiance += calculatePointLight(pointLights[i], normalUnit, viewDirection);
	}

	irradiance += calculateSpotLight(spotLight, normalUnit, viewDirection);
	
	// debug
//	irradiance += texture(material.specularColor, texCoord).rgb;

	gl_FragColor = vec4(irradiance, 1.0f);
}