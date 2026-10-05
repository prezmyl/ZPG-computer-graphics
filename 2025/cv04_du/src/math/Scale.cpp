#include "math/Scale.h"
#include <glm/gtc/matrix_transform.hpp>
glm::mat4 Scale::matrix() const { return glm::scale(glm::mat4(1.f), s_); }
