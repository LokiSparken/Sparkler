#version 330 core

in vec2 texCoords;

uniform sampler2D screenTexture;

void main()
{ 
	// 1. use the color attachment of the new framebuffer directly
//	gl_FragColor = texture(screenTexture, texCoords);

	// 2. reverse (produce a beautiful result!)
//	gl_FragColor = vec4(vec3(1.0 - texture(screenTexture, texCoords)), 1.0);

	// 3. grey
	vec4 fragColor = texture(screenTexture, texCoords);
    float average = 0.2126 * fragColor.r + 0.7152 * fragColor.g + 0.0722 * fragColor.b;
    gl_FragColor = vec4(average, average, average, 1.0);
}