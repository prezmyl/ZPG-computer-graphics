//
// Created by xpolas on 10/15/26.
//

#ifndef PROJECT_SCALE_H
#define PROJECT_SCALE_H
#include "Transformation.h"


class Scale : public Transformation {
private:
    glm::vec3 factor;

public:
    Scale(glm::vec3 factor);
    glm::mat4 getMatrix() const override;
};


#endif //PROJECT_SCALE_H
