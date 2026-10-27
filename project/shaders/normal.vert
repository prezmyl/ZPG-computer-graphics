#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out vec3 vertexColor;
out vec3 surfaceNormal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main()
{
    surfaceNormal = normal;
    // Convert normal components from [-1, 1] to [0, 1]
    //vertexColor = normal * 0.5 + 0.5;

    if (gl_VertexID > 36443) {
        vertexColor = vec3(0.0, 1.0, 0.0); // Green leaves
    } else {
        vertexColor = vec3(1.0, 0.0, 0.0); // Red trunk
    }

    // Transform vertex into clip space
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);
}