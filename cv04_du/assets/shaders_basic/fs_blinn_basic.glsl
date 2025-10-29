#version 330 core
in vec3 vNormal; in vec3 vWorldPos;
out vec4 fragColor;
uniform vec3 uKa, uKd, uKs;
uniform float uShininess;
uniform vec3 uLightPos;
uniform vec3 uViewPos;
void main(){
    vec3 N = normalize(vNormal);
    vec3 L = normalize(uLightPos - vWorldPos);
    vec3 V = normalize(uViewPos - vWorldPos);
    vec3 H = normalize(L + V);
    float diff = max(dot(N,L), 0.0);
    float spec = diff > 0.0 ? pow(max(dot(N,H),0.0), uShininess) : 0.0;
    vec3 col = uKa + uKd*diff + uKs*spec;
    fragColor = vec4(col,1.0);
}
