#pragma once
#include <glm/glm.hpp>

struct AbstractTransformation {
    virtual ~AbstractTransformation() = default;
    virtual glm::mat4 matrix() const = 0;
};

struct Translate : AbstractTransformation {
    glm::vec3 t;
    explicit Translate(glm::vec3 t) : t(t) {}
    glm::mat4 matrix() const override;
};
struct Rotate : AbstractTransformation {
    float deg; glm::vec3 axis;
    Rotate(float deg, glm::vec3 axis) : deg(deg), axis(axis) {}
    glm::mat4 matrix() const override;
};
struct Scale : AbstractTransformation {
    glm::vec3 s;
    explicit Scale(glm::vec3 s) : s(s) {}
    glm::mat4 matrix() const override;
};
