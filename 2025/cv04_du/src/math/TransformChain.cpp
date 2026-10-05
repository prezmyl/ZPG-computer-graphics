#include "math/TransformChain.h"

glm::mat4 TransformChain::combined() const {
    glm::mat4 M(1.f);
    // Column-vector konvence: aplikovat T1 pak T2, výsledná M = T2 * T1.
    // Proto behem pruchodu **zleva nasobenis**: M = T_i * M.
    for (const auto& t : items_) {
        M = t->matrix() * M;
    }
    return M;
}
