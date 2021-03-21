#version 330 core

in vec3 color;
in vec2 texCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

uniform float blendValue;

out vec4 fragColor;

void main()
{
//	fragColor = texture(texture2, texCoord);
	fragColor = mix(texture(texture1, texCoord), texture(texture2, vec2(texCoord.x - 0.34f * texCoord.y, texCoord.y)), 0.6);
//	fragColor = mix(texture(texture1, texCoord), texture(texture2, vec2(texCoord.x, texCoord.y)), 0.6);
//	fragColor = mix(texture(texture1, texCoord), texture(texture2, vec2(texCoord.x - 0.34f * texCoord.y, texCoord.y)), blendValue);
//	fragColor = vec4(color.xyz, 1.0f) * fragColor;
}
