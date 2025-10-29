#pragma once
#include <GLFW/glfw3.h>
#include "Scene.h"
#include "PNArrayModel.h"
#include "Transform.h"
#include "TransformChain.h"

// 1) Rotujuci trojuhlenik (VS expects pos+normal => normalaa bude (0,0,1))
class RotatingTriangleScene : public SimpleScene {
public:
    explicit RotatingTriangleScene(ShaderProgram* progColor) : progColor_(progColor) {
        static const float tri[] = {
            // pos.xyz           normal.xyz
            -0.5f,-0.5f,0.f,    0,0,1,
             0.5f,-0.5f,0.f,    0,0,1,
             0.0f, 0.5f,0.f,    0,0,1
        };
        triModel_ = std::make_unique<PNArrayModel>(tri);

        auto chain = std::make_shared<TransformChain>();
        spin_ = std::make_shared<Rotate>(0.f, glm::vec3(0,0,1));
        chain->add(spin_);

        auto d = std::make_unique<DrawableObject>();
        d->model = triModel_.get();
        d->program = progColor_;
        d->transform = chain;
        add(std::move(d));
    }

    void update(float /*dt*/) override {   // (float)glfwGetTime()*50.f
        spin_->deg = (float)glfwGetTime() * 50.f;
    }

private:
    ShaderProgram* progColor_;
    std::unique_ptr<PNArrayModel> triModel_;
    std::shared_ptr<Rotate> spin_;
};

// 2) 4 koule
class FourSpheresScene : public SimpleScene {
public:
    FourSpheresScene(ShaderProgram* progNormal, Model* sphereModel) {
        for (glm::vec3 p : { glm::vec3(1,0,0), {-1,0,0}, {0,1,0}, {0,-1,0} }) {
            auto chain = std::make_shared<TransformChain>();
            chain->add(std::make_shared<Translate>(p * 1.2f));
            chain->add(std::make_shared<Scale>(glm::vec3(0.45f)));

            auto d = std::make_unique<DrawableObject>();
            d->model = sphereModel;
            d->program = progNormal;
            d->transform = chain;
            add(std::move(d));
        }
    }
};
