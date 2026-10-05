#pragma once
#include <glm/glm.hpp>

class Camera {
public:
    explicit Camera(glm::vec3 pos = {0.f, 1.6f, 3.f}, float yawDeg=-90.f, float pitchDeg=0.f);

    glm::mat4 view() const;
    glm::mat4 proj(float fovyDeg, float aspect, float zNear, float zFar) const;

    void processKeyboard(bool fwd, bool back, bool left, bool right, float dt);
    void processMouse(float dx, float dy, bool constrainPitch = true);

    glm::vec3 position() const { return Position; }
    float yaw() const { return Yaw; }
    float pitch() const { return Pitch; }

private:
    void updateVectors();

    glm::vec3 Position;
    glm::vec3 Front{0,0,-1}, Up{0,1,0}, Right{1,0,0}, WorldUp{0,1,0};
    float Yaw, Pitch;
    float MoveSpeed = 3.0f, MouseSensitivity = 0.1f;
};
