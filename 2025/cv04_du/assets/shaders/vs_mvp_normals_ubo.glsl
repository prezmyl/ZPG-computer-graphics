#version 330 core
layout(std140, binding = 0) uniform CameraBlock {
    mat4 uView; mat4 uProj; mat4 uViewProj; vec4 uViewPos;
};
layout(std140, binding = 1) uniform MaterialBlock {
    vec4 uKa; vec4 uKd; vec4 uKs; float uShininess; float _pad0, _pad1, _pad2;
};
struct Light { vec4 pos; vec4 color; };
layout(std140, binding = 2) uniform LightBlock {
    Light lights[4]; ivec4 lightCount;
};

layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aNormal;

uniform mat4 uModel;

out vec3 vNormal;
out vec3 vWorldPos;

void main(){
    vec4 worldPos = uModel * vec4(aPos, 1.0);
    vWorldPos = worldPos.xyz;

    mat3 normalMatrix = mat3(transpose(inverse(uModel)));
    vNormal = normalize(normalMatrix * aNormal);

    gl_Position = uViewProj * worldPos;
}
