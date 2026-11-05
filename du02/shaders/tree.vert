#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out vec3 vertexColor;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main()
{
    if (gl_VertexID > 36443) {
        vertexColor = vec3(0.0, 1.0, 0.0);
    } else {
        vertexColor = vec3(1.0, 0.0, 0.0);
    }

    gl_Position =
        projectionMatrix *
        viewMatrix *
        modelMatrix *
        vec4(position, 1.0);
}