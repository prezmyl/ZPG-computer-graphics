//
// Created by xpolas on 10/27/26.
//

#include "RotationAnimation.h"

RotationAnimation::RotationAnimation(Rotation *rotation, float angularSpeed) : rotation(rotation), angularSpeed(angularSpeed){
}

void RotationAnimation::update(float time) {
    rotation->setAngle(angularSpeed * time);
}
