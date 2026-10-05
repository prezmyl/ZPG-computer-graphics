#include "scene/SpheresScene.h"
#include <glm/gtc/matrix_transform.hpp>
#include <fstream>
#include <sstream>
#include <string>
#include "meshes/sphere.h"   // očekává const float sphere[]; interleaved pos+norm

static std::string slurp(const char* p){ std::ifstream f(p); std::stringstream ss; ss<<f.rdbuf(); return ss.str(); }
static ShaderProgram loadProg(const char* vsPath, const char* fsPath){
    std::string vs=slurp(vsPath), fs=slurp(fsPath);
    return ShaderProgram::fromSources(vs.c_str(), fs.c_str());
}

SpheresScene::SpheresScene(){
    const size_t floats = sizeof(sphere)/sizeof(float);
    const size_t stride = 6;                    // 3 pos + 3 norm
    const size_t vtxCnt = floats / stride;      // glDrawArrays(..., vtxCnt)
    sphere_ = std::make_unique<MeshModel>(sphere, vtxCnt, true, (int)stride);

    spConst_   = loadProg("assets/shaders_basic/vs_posnorm_basic.glsl","assets/shaders_basic/fs_constant_basic.glsl");
    spLambert_ = loadProg("assets/shaders_basic/vs_posnorm_basic.glsl","assets/shaders_basic/fs_lambert_basic.glsl");
    spPhong_   = loadProg("assets/shaders_basic/vs_posnorm_basic.glsl","assets/shaders_basic/fs_phong_basic.glsl");
    spBlinn_   = loadProg("assets/shaders_basic/vs_posnorm_basic.glsl","assets/shaders_basic/fs_blinn_basic.glsl");
}

void SpheresScene::update(float){}

void SpheresScene::render(const glm::mat4& V, const glm::mat4& P){
    using glm::vec3; using glm::mat4;
    mat4 M1 = glm::translate(mat4(1.f), vec3(-1.5f,0.f,0.f));  // Constant
    mat4 M2 = glm::translate(mat4(1.f), vec3( 1.5f,0.f,0.f));  // Lambert
    mat4 M3 = glm::translate(mat4(1.f), vec3( 0.f,0.f,-1.5f)); // Phong
    mat4 M4 = glm::translate(mat4(1.f), vec3( 0.f,0.f, 1.5f)); // Blinn

    auto draw=[&](ShaderProgram& sp, const mat4& M, vec3 Ka, vec3 Kd, vec3 Ks, float shin){
        sp.use();
        sp.setUniform("uModel", M);
        sp.setUniform("uView",  V);
        sp.setUniform("uProj",  P);
        sp.setUniform("uLightPos", lightPos_);
        sp.setUniform("uViewPos",  viewPos_);
        sp.setUniform("uKa", Ka); sp.setUniform("uKd", Kd);
        sp.setUniform("uKs", Ks); sp.setUniform("uShininess", shin);
        sphere_->draw();
    };

    // Constant
    spConst_.use();
    spConst_.setUniform("uModel", M1);
    spConst_.setUniform("uView",  V);
    spConst_.setUniform("uProj",  P);
    spConst_.setUniform("uKd", vec3(0.9f,0.25f,0.2f));
    sphere_->draw();

    // Lambert / Phong / Blinn
    draw(spLambert_, M2, vec3(0.05f), vec3(0.3f,0.8f,0.3f), vec3(0.0f),  1.f);
    draw(spPhong_,   M3, vec3(0.05f), vec3(0.8f,0.6f,0.3f), vec3(0.8f), 32.f);
    draw(spBlinn_,   M4, vec3(0.05f), vec3(0.3f,0.5f,0.9f), vec3(0.8f), 32.f);
}

void SpheresScene::setViewPos(const glm::vec3& p){ viewPos_ = p; }
