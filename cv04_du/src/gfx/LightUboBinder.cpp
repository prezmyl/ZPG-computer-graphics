#include "LightUboBinder.h"
#include <algorithm>

LightUboBinder::LightUboBinder()
: ubo_(sizeof(Std140LightBlock), Technique::LIGHT_BINDING) {}

void LightUboBinder::setLights(const std::vector<Light>& lights){
    cpu_ = lights;
    if(cpu_.size() > 4) cpu_.resize(4);
}

void LightUboBinder::upload(){
    Std140LightBlock b{};
    int n = (int)std::min<size_t>(cpu_.size(), 4);
    for(int i=0;i<n;i++){
        b.lights[i].pos   = glm::vec4(cpu_[i].position, 1.0f);
        b.lights[i].color = glm::vec4(cpu_[i].color,    1.0f);
    }
    b.count.x = n;
    ubo_.update(&b, sizeof(b), 0);
}
