#pragma once
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include "DrawableObject.h"

class Scene {
public:
    virtual ~Scene() = default;
    virtual void update(float /*dt*/) {}
    virtual void render(const glm::mat4& V, const glm::mat4& P) = 0;
};

class SimpleScene : public Scene {
public:
    void add(std::unique_ptr<DrawableObject> d) { objs_.push_back(std::move(d)); }
    void render(const glm::mat4& V, const glm::mat4& P) override {
        for (auto& o : objs_) o->draw(V, P);
    }
protected:
    std::vector<std::unique_ptr<DrawableObject>> objs_;
};

class SceneManager {
public:
    void add(std::unique_ptr<Scene> s) { scenes_.push_back(std::move(s)); }
    Scene* active() {
        if (scenes_.empty()) return nullptr;
        if (current_ < 0 || current_ >= (int)scenes_.size()) current_ = 0;
        return scenes_[current_].get();
    }
    void switchTo(int idx){
        if (idx >= 0 && idx < (int)scenes_.size()) current_ = idx;
    }
private:
    std::vector<std::unique_ptr<Scene>> scenes_;
    int current_ = 0;
};
