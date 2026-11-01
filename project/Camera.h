//
// Created by xpolas on 11/1/26.
//

#ifndef PROJECT_CAMERA_H
#define PROJECT_CAMERA_H
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include <glm/ext/matrix_float4x4.hpp>


class Camera {
private:
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 up;

    float fov;
    float aspectRatio;
    float near;
    float far;

public:
    Camera(glm::vec3 position, glm::vec3 direction, glm::vec3 up, float fov, float aspectRatio, float near, float far);

    glm::vec3 getCenter() const;
    glm::mat4 getViewMatrix() const; //lookAt(position, center, up)
    glm::mat4 getProjectionMatrix() const; //
    void resize(int width, int height);

};


#endif //PROJECT_CAMERA_H
