#version 330 core
out vec4 fragColor;
uniform vec3 uKd; // barva
void main(){ fragColor = vec4(uKd, 1.0); }
