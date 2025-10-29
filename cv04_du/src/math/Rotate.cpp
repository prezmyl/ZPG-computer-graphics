#include "Rotate.h"
#include <glm/gtc/matrix_transform.hpp>
glm::mat4 Rotate::matrix() const {
    return glm::rotate(glm::mat4(1.f), glm::radians(degrees()), axis());
}
