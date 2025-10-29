#pragma once
#include <glm/glm.hpp>
#include "Ubo.h"
#include "Technique.h"

class Material {
public:
    Material();

    void setKa(const glm::vec3& v) { Ka = v; dirty_ = true; }
    void setKd(const glm::vec3& v) { Kd = v; dirty_ = true; }
    void setKs(const glm::vec3& v) { Ks = v; dirty_ = true; }
    void setShininess(float s)     { Shininess = s; dirty_ = true; }

    void uploadIfNeeded();
    void bind() const { ubo_.bindBase(); }

private:
    struct Std140Material {
        glm::vec4 Ka;
        glm::vec4 Kd;
        glm::vec4 Ks;
        float Shininess;
        float _pad0, _pad1, _pad2;
    };

    glm::vec3 Ka{0.05f};
    glm::vec3 Kd{0.8f};
    glm::vec3 Ks{0.2f};
    float     Shininess{32.f};

    bool dirty_ = true;
    Ubo  ubo_;
};
