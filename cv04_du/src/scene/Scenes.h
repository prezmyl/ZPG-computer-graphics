#pragma once
#include <GLFW/glfw3.h>
#include "scene/Scene.h"
#include "models/TriangleModel.h"
#include "math/TransformChain.h"
#include "math/Rotate.h"

class RotatingTriangleScene : public SimpleScene {
public:
    explicit RotatingTriangleScene(ShaderProgram* prog)
    : prog_(prog) {
        tri_ = std::make_unique<TriangleModel>();
        auto chain = std::make_shared<TransformChain>();
        spin_ = std::make_shared<Rotate>(0.f, glm::vec3(0,0,1));
        chain->add(spin_);

        auto d = std::make_unique<DrawableObject>();
        d->model     = tri_.get();
        d->program   = prog_;
        d->transform = chain;
        add(std::move(d));
    }
    void update(float) override { spin_->setDegrees((float)glfwGetTime() * 50.f); }
private:
    ShaderProgram* prog_;
    std::unique_ptr<TriangleModel> tri_;
    std::shared_ptr<Rotate> spin_;
};

class EmptyScene : public Scene {
public:
    void render(const glm::mat4&, const glm::mat4&) override {}
};
