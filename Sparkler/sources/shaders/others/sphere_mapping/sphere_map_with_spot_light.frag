#version 330 core

in vec3 normal;
in vec3 fragPosition;
in vec2 texCoords;

struct Material
{
	sampler2D textureDiffuse1;
	sampler2D textureSpecular1;
	float shininess;
};

uniform Material material;

struct SpotLight
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	
	vec3 position;
	vec3 direction;
	float cutoff;
	float outerCutoff;
};

uniform SpotLight spotLight;
uniform vec3 viewPosition;

void main()
{
	vec3 tracingDirection = vec3(fragPosition - spotLight.position);
	float cos_theta   = dot(normalize(tracingDirection), normalize(spotLight.direction));
	float epsilon     = spotLight.cutoff - spotLight.outerCutoff;
	float attenuation = clamp((cos_theta - spotLight.outerCutoff) / epsilon, 0.0f, 1.0f);

	float ambientCoeff = 0.1f;
	vec3 ambient = vec3(texture(material.textureDiffuse1, texCoords)) * spotLight.ambient;

	vec3 normalUnit = normalize(normal);
	vec3 lightDirection = normalize(spotLight.position - fragPosition);
	float diffuseCoeff = max(dot(lightDirection, normalUnit), 0.0);
	vec3 diffuse = vec3(texture(material.textureDiffuse1, texCoords)) * diffuseCoeff * spotLight.diffuse * attenuation;

	vec3 viewDirection = normalize(fragPosition - viewPosition);
	vec3 reflectDirection = reflect(-viewDirection, normalUnit);
	float specularCoeff = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);
	vec3 specularColor = vec3(texture(material.textureSpecular1, texCoords));
	vec3 specular = specularColor * specularCoeff * spotLight.specular * attenuation;

	vec3 irradiance = ambient + diffuse + specular;
	gl_FragColor = vec4(irradiance, 1.0f);
}