#pragma once
#include "scene/Scene.h"
#include "gfx/ShaderProgram.h"
#include "models/MeshModel.h"
#include <memory>
#include <glm/glm.hpp>

class SpheresScene : public Scene {
public:
    SpheresScene();
    void update(float dt) override;
    void render(const glm::mat4& V, const glm::mat4& P) override;
    void setViewPos(const glm::vec3&) override;
private:
    std::unique_ptr<MeshModel> sphere_;
    ShaderProgram spConst_, spLambert_, spPhong_, spBlinn_;
    glm::vec3 lightPos_{0.f, 1.5f, 0.f};
    glm::vec3 viewPos_{0.f, 1.6f, 4.f};
};
