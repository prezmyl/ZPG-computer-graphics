#version 330 core

in vec3 vertexColor;
in vec3 surfaceNormal;

out vec4 FragColor;

void main()
{
    float shade = 0.3 + 0.7 * abs(normalize(surfaceNormal).y);

    FragColor = vec4(shade * vertexColor, 1.0);
}