//
// Created by xpolas on 10/15/26.
//

#ifndef PROJECT_COMPOSITETRANSFORMATION_H
#define PROJECT_COMPOSITETRANSFORMATION_H
#include <memory>
#include <vector>

#include "Transformation.h"


class CompositeTransformation : public Transformation {
private:
    std::vector<std::unique_ptr<Transformation>> transformations;
public:
    CompositeTransformation() = default;
    void addTransformation(std::unique_ptr<Transformation> transformation);

    glm::mat4 getMatrix() const override;
};


#endif //PROJECT_COMPOSITETRANSFORMATION_H
