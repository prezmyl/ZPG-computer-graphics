#pragma once
#include <vector>
#include "Ubo.h"
#include "Technique.h"
#include "Light.h"

class LightUboBinder {
public:
    explicit LightUboBinder();

    void setLights(const std::vector<Light>& lights);
    void upload();

private:
    struct Std140Light { glm::vec4 pos; glm::vec4 color; };
    struct Std140LightBlock {
        Std140Light lights[4];
        glm::ivec4 count; // .x = count
    };

    std::vector<Light> cpu_;
    Ubo ubo_;
};
