#pragma once
#include <glm/glm.hpp>

// cista abstr tr. ->rozhrani pro libovolne T/R/S (4×4 v projektiv. prostoru)
class AbstractTransformation {
public:
    virtual ~AbstractTransformation() = default;
    virtual glm::mat4 matrix() const = 0;
};
