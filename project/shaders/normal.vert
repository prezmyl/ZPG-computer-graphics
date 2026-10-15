#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out vec3 vertexColor;

uniform mat4 modelMatrix;

void main()
{
    // Convert normal components from [-1, 1] to [0, 1]
    vertexColor = normal * 0.5 + 0.5;

    // Transform vertex into clip space
    gl_Position = modelMatrix * vec4(position, 1.0);
}