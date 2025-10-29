#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

Camera::Camera(glm::vec3 pos, float yawDeg, float pitchDeg)
: Position(pos), Yaw(yawDeg), Pitch(pitchDeg) {
    updateVectors();
}

glm::mat4 Camera::view() const {
    return glm::lookAt(Position, Position + Front, Up);
}

glm::mat4 Camera::proj(float fovyDeg, float aspect, float zNear, float zFar) const {
    return glm::perspective(glm::radians(fovyDeg), aspect, zNear, zFar);
}

void Camera::processKeyboard(bool fwd, bool back, bool left, bool right, float dt){
    float v = MoveSpeed * dt;
    if(fwd)  Position += glm::vec3(Front.x,0,Front.z) * v;
    if(back) Position -= glm::vec3(Front.x,0,Front.z) * v;
    if(left) Position -= Right * v;
    if(right)Position += Right * v;
}

void Camera::processMouse(float dx, float dy, bool constrainPitch){
    dx *= MouseSensitivity; dy *= MouseSensitivity;
    Yaw += dx; Pitch += dy;
    if(constrainPitch){ if(Pitch>89.f)Pitch=89.f; if(Pitch<-89.f)Pitch=-89.f; }
    updateVectors();
}

void Camera::updateVectors(){
    float yawR = glm::radians(Yaw), pitchR = glm::radians(Pitch);
    glm::vec3 f{ cosf(pitchR)*cosf(yawR), sinf(pitchR), cosf(pitchR)*sinf(yawR) };
    Front = glm::normalize(f);
    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up    = glm::normalize(glm::cross(Right, Front));
}
