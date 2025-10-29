#include "Technique.h"

std::shared_ptr<Technique> Technique::createFromFiles(const std::string& vs, const std::string& fs, std::string* err){
    auto prog = std::make_shared<ShaderProgram>();
    if(!prog->loadFromFiles(vs, fs, err)){
        return nullptr;
    }
    prog->bindUniformBlock("CameraBlock",   CAMERA_BINDING);
    prog->bindUniformBlock("MaterialBlock", MATERIAL_BINDING);
    prog->bindUniformBlock("LightBlock",    LIGHT_BINDING);

    auto t = std::make_shared<Technique>();
    t->program = std::move(prog);
    return t;
}
