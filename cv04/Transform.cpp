#include "Transform.h"
#include <glm/gtc/matrix_transform.hpp>

glm::mat4 Translate::matrix() const { return glm::translate(glm::mat4(1.f), t); }
glm::mat4 Rotate::matrix()    const { return glm::rotate(glm::mat4(1.f), glm::radians(deg), axis); }
glm::mat4 Scale::matrix()     const { return glm::scale(glm::mat4(1.f), s); }
