#version 330 core
in vec3 vNormal; in vec3 vWorldPos;
out vec4 fragColor;
uniform vec3 uKa, uKd;
uniform vec3 uLightPos;
void main(){
    vec3 N = normalize(vNormal);
    vec3 L = normalize(uLightPos - vWorldPos);
    float diff = max(dot(N,L), 0.0);
    vec3 col = uKa + uKd * diff;
    fragColor = vec4(col,1.0);
}
