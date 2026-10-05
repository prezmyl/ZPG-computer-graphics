#pragma once
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "AbstractTransformation.h"

// tady drzim usporadane transformace -> odpovida modelove matici M z pradnasek
class TransformChain {
public:
    void add(std::shared_ptr<AbstractTransformation> t) { items_.push_back(std::move(t)); }
    void clear() { items_.clear(); }
    glm::mat4 combined() const;   // M = Tn * ... * T2 * T1

private:
    std::vector<std::shared_ptr<AbstractTransformation>> items_;
};
