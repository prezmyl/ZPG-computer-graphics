#include "core/DrawableObject.h"
#include "core/Model.h"
#include "gfx/ShaderProgram.h"
#include "math/TransformChain.h"

void DrawableObject::draw(const glm::mat4& V, const glm::mat4& P) const {
    if (!model || !program) return;
    glm::mat4 M(1.f);
    if (transform) M = transform->combined();

    program->use();
    program->setUniform("uModel", M);
    program->setUniform("uView",  V);
    program->setUniform("uProj",  P);
    model->draw();
}
