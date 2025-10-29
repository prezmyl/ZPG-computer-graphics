#pragma once
#include <glm/glm.hpp>
#include "Ubo.h"
#include "Technique.h"
#include "Camera.h"

class CameraUboBinder {
public:
    explicit CameraUboBinder();

    void update(const Camera& cam, float fovyDeg, float aspect, float zNear, float zFar);
    const Ubo& ubo() const { return ubo_; }

private:
    struct Std140Camera {
        glm::mat4 V;
        glm::mat4 P;
        glm::mat4 VP;
        glm::vec4 ViewPos;
    };
    Ubo ubo_;
};
