#version 330 core
layout(std140, binding = 0) uniform CameraBlock { mat4 uView; mat4 uProj; mat4 uViewProj; vec4 uViewPos; };
layout(std140, binding = 1) uniform MaterialBlock { vec4 uKa; vec4 uKd; vec4 uKs; float uShininess; float _pad0,_pad1,_pad2; };
layout(std140, binding = 2) uniform LightBlock { struct Light { vec4 pos; vec4 color; }; Light lights[4]; ivec4 lightCount; };

out vec4 fragColor;
uniform vec3 uColor;
void main(){ fragColor = vec4(uColor, 1.0); }
