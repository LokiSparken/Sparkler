#version 330 core
in vec3 normal;
in vec3 position;

uniform vec3 cameraPosition;
uniform samplerCube skybox;

void main()
{             
    vec3 I = normalize(position - cameraPosition);
    vec3 R = reflect(I, normalize(normal));
    gl_FragColor = vec4(texture(skybox, R).rgb, 1.0);
}