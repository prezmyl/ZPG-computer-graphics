//
// Created by xpolas on 10/27/26.
//

#ifndef PROJECT_ANIMATION_H
#define PROJECT_ANIMATION_H

class Animation {
public:
    virtual void update(float time) const = 0;
    virtual ~Animation() = default;
};

#endif //PROJECT_ANIMATION_H
