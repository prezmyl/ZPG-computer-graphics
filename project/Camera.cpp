//
// Created by xpolas on 11/1/26.
//

#include "Camera.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

Camera::Camera(glm::vec3 position, glm::vec3 direction, glm::vec3 up, float fov, float aspectRatio, float near,
               float far) : position(position), direction(direction), up(up), fov(fov), aspectRatio(aspectRatio), near(near), far(far) {
}

glm::vec3 Camera::getCenter() const {
    return position + direction;
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, getCenter(), up);
}

glm::mat4 Camera::getProjectionMatrix() const {
    return glm::perspective(glm::radians(fov), aspectRatio, near, far);
}

void Camera::resize(int width, int height) {
    //prevent 0 division if the window is minimized, thus the height or widht is 0
    if (width <= 0 || height <= 0) {return;}
    aspectRatio = static_cast<float>(width) / static_cast<float>(height);

}

void Camera::moveForward(float distance) {
    //leveled movment should ignore the y coordivate and the vector should be normalized
    glm::vec3 forward = glm::normalize(glm::vec3(direction.x, 0.0f, direction.z));
    position = position + forward * distance;
}

void Camera::moveRight(float distance) {
    glm::vec3 forward = glm::normalize(glm::vec3(direction.x, 0.0f, direction.z));
    glm::vec3 right = glm::normalize(glm::cross(forward, up)); //perpendicular to forward and up(used as helper vector)
    position = position + right * distance;
}

void Camera::rotate(float yawDelta, float pitchDelta) {
    float yaw = glm::atan(direction.z, direction.x);
    float pitch = glm::asin(direction.y);

    yaw = yaw + yawDelta;
    pitch = pitch + pitchDelta;

    //limit the pitch due to mathematics restrains on calculation
    pitch = glm::clamp(pitch, -89.0f, 89.0f);

    direction.x = glm::cos(pitch) * glm::cos(yaw);
    direction.y = glm::sin(pitch);
    direction.z = glm::cos(pitch) * glm::sin(yaw);
}

/*
Camera& Camera::getCamera() {
    return *this;
}*/
