#version 330 core

in vec2 texCoords;

uniform sampler2D textureDiffuse1;

float near = 0.1;
float far = 100.0f;

float linearizeDepth(float depth)
{
	float ndc = depth * 2.0f - 1.0f;
	return (2.0 * near * far) / (far + near - ndc * (far - near));
}

void main()
{
//    gl_FragColor = texture(textureDiffuse1, texCoords);
	float depth = linearizeDepth(gl_FragCoord.z) / far;
	gl_FragColor = vec4(vec3(depth), 1.0f);
}