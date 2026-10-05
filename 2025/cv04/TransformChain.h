#pragma once
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include "Transform.h"

class TransformChain {
public:
    void add(std::shared_ptr<AbstractTransformation> t) { items_.push_back(std::move(t)); }
    glm::mat4 modelMatrix() const {
        glm::mat4 M(1.f);
        for (auto& t : items_) M = M * t->matrix();
        return M;
    }
private:
    std::vector<std::shared_ptr<AbstractTransformation>> items_;
};
