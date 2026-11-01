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

glm::mat4 Camera::resize(int width, int height) {
    glViewport(0, 0, width, height);
    aspectRatio = (float) width / (float) height;
    return getProjectionMatrix();
}
