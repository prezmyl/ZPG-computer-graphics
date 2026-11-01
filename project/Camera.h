//
// Created by xpolas on 11/1/26.
//

#ifndef PROJECT_CAMERA_H
#define PROJECT_CAMERA_H
#include <glm/vec3.hpp>
#include <glm/ext/matrix_float4x4.hpp>


class Camera {
private:
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 up;

    float fov;

public:
    Camera(glm::vec3 position, glm::vec3 direction, glm::vec3 up, float fov);
    glm::vec3 getCenter;
    glm::mat4 getViewMatrix(); //lookAt(position, center, up)
    glm::mat4 getProjectionMatrix(); //
};


#endif //PROJECT_CAMERA_H
