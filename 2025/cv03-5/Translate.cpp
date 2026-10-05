#include "Translate.h"
#include <glm/gtc/matrix_transform.hpp>
glm::mat4 Translate::matrix() const { return glm::translate(glm::mat4(1.f), d_); }
