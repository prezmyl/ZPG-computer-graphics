#pragma once
#include "scene/Scene.h"
#include "models/MeshModel.h"
#include "gfx/ShaderProgram.h"
#include <memory>
#include <glm/glm.hpp>

class SpheresScene : public Scene {
    void setViewPos(const glm::vec3&) override;
public:
    SpheresScene();
    void update(float dt) override;
    void render(const glm::mat4& V, const glm::mat4& P) override;
private:
    std::unique_ptr<MeshModel> sphere_;
    ShaderProgram spConst_, spLambert_, spPhong_, spBlinn_;
    glm::vec3 lightPos_{0.f, 1.5f, 0.f};
    glm::vec3 camPos_{0.f, 1.6f, 3.f}; // pro Phong/Blinn (uViewPos)
};
