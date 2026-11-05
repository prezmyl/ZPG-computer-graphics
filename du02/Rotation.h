//
// Created by xpolas on 10/15/26.
//

#ifndef PROJECT_ROTATION_H
#define PROJECT_ROTATION_H

#include "Transformation.h"

class Rotation : public Transformation {
private:
    glm::vec3 axis;
    float angle; //rads

public:
    Rotation(glm::vec3 axis, float angle);
    glm::mat4 getMatrix() const override;
    void setAngle(float angle);

};


#endif //PROJECT_ROTATION_H
