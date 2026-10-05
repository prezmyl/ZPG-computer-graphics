#include "gfx/Material.h"

Material::Material()
: ubo_(sizeof(Std140Material), Technique::MATERIAL_BINDING)
{
    uploadIfNeeded();
}

void Material::uploadIfNeeded(){
    if(!dirty_) return;
    Std140Material m{};
    m.Ka = glm::vec4(Ka, 1.0f);
    m.Kd = glm::vec4(Kd, 1.0f);
    m.Ks = glm::vec4(Ks, 1.0f);
    m.Shininess = Shininess;
    ubo_.update(&m, sizeof(m), 0);
    dirty_ = false;
}
