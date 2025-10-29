#include "gfx/CameraUboBinder.h"

CameraUboBinder::CameraUboBinder()
: ubo_(sizeof(Std140Camera), Technique::CAMERA_BINDING) {}

void CameraUboBinder::update(const Camera& cam, float fovyDeg, float aspect, float zNear, float zFar){
    Std140Camera c{};
    c.V  = cam.view();
    c.P  = cam.proj(fovyDeg, aspect, zNear, zFar);
    c.VP = c.P * c.V;
    c.ViewPos = glm::vec4(cam.position(), 1.0f);
    ubo_.update(&c, sizeof(c), 0);
}
