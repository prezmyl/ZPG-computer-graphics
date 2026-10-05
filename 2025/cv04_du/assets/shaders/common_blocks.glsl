// Bindingy: Camera=0, Material=1, Light=2
layout(std140, binding = 0) uniform CameraBlock {
    mat4 uView;
    mat4 uProj;
    mat4 uViewProj;
    vec4 uViewPos;
};

layout(std140, binding = 1) uniform MaterialBlock {
    vec4 uKa;
    vec4 uKd;
    vec4 uKs;
    float uShininess;
    float _pad0, _pad1, _pad2;
};

struct Light { vec4 pos; vec4 color; };
layout(std140, binding = 2) uniform LightBlock {
    Light lights[4];
    ivec4 lightCount;
};
