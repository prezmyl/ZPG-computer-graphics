//
// Created by xpolas on 10/27/26.
//

#ifndef PROJECT_ROTATIONANIMATION_H
#define PROJECT_ROTATIONANIMATION_H
#include "Animation.h"
#include "Rotation.h"


class RotationAnimation : public Animation{
private:
    Rotation* rotation; //which rotation is concerned
    float angularSpeed;

public:
    RotationAnimation(Rotation* rotation, float angularSpeed);
    void update(float time) override;
};


#endif //PROJECT_ROTATIONANIMATION_H
