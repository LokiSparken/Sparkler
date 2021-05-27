#version 330 core

in vec2 texCoords;

uniform sampler2D screenTexture;

const float offset = 1.0 / 300.0;

void main()
{
    // sample offset
	vec2 offsets[9] = vec2[]
    (
        vec2(-offset,  offset), // left top
        vec2( 0.0f,    offset), // center top
        vec2( offset,  offset), // right top
        vec2(-offset,  0.0f),   // left
        vec2( 0.0f,    0.0f),   // center
        vec2( offset,  0.0f),   // right
        vec2(-offset, -offset), // left bottom
        vec2( 0.0f,   -offset), // center bottom
        vec2( offset, -offset)  // right bottom
    );

    float kernel[9] = float[]
    (
        // sharpen kernel
//        -1, -1, -1,
//        -1,  9, -1,
//        -1, -1, -1
        // blur kernel
//        1.0 / 16, 2.0 / 16, 1.0 / 16,
//        2.0 / 16, 4.0 / 16, 2.0 / 16,
//        1.0 / 16, 2.0 / 16, 1.0 / 16
        // edge detect
        1, 1, 1,
        1, -8, 1,
        1, 1, 1
    );

    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(screenTexture, texCoords.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];

    gl_FragColor = vec4(col, 1.0);
}