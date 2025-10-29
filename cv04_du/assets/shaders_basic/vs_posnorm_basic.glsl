#version 330 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aNormal;
uniform mat4 uModel, uView, uProj;
out vec3 vNormal;
out vec3 vWorldPos;
void main(){
    vec4 world = uModel * vec4(aPos,1.0);
    vWorldPos = world.xyz;
    mat3 N = mat3(transpose(inverse(uModel)));
    vNormal = normalize(N * aNormal);
    gl_Position = uProj * uView * world;
}
