//
// Created by pol0511 on 10/14/26.
//

#ifndef PROJECT_TRANSFORMATION_H
#define PROJECT_TRANSFORMATION_H

#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_transform.hpp>

class Transformation {
public:
    virtual ~Transformation() = default;
    virtual glm::mat4 getMatrix() const = 0;
};


#endif //PROJECT_TRANSFORMATION_H
