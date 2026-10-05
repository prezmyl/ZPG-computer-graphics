#version 330 core
layout(std140, binding = 0) uniform CameraBlock { mat4 uView; mat4 uProj; mat4 uViewProj; vec4 uViewPos; };
layout(std140, binding = 1) uniform MaterialBlock { vec4 uKa; vec4 uKd; vec4 uKs; float uShininess; float _pad0,_pad1,_pad2; };
struct Light { vec4 pos; vec4 color; };
layout(std140, binding = 2) uniform LightBlock { Light lights[4]; ivec4 lightCount; };

in vec3 vNormal;
in vec3 vWorldPos;
out vec4 fragColor;

void main(){
    vec3 N = normalize(vNormal);
    vec3 V = normalize(uViewPos.xyz - vWorldPos);
    vec3 col = uKa.rgb;
    int n = lightCount.x;
    for(int i=0;i<n;i++){
        vec3 L = normalize(lights[i].pos.xyz - vWorldPos);
        vec3 R = reflect(-L, N);
        float diff = max(dot(N, L), 0.0);
        float spec = (diff>0.0) ? pow(max(dot(R, V), 0.0), uShininess) : 0.0;
        col += (uKd.rgb * diff + uKs.rgb * spec) * lights[i].color.rgb;
    }
    fragColor = vec4(col, 1.0);
}
