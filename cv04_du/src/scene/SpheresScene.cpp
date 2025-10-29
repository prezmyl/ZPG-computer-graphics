#include "scene/SpheresScene.h"
#include <glm/gtc/matrix_transform.hpp>
#include "meshes/sphere.h"

static ShaderProgram loadProg(const char* vsPath, const char* fsPath){
    // jednoduchý loader ze souboru (můžeš nahradit Technique::createFromFiles)
    auto slurp = [](const char* p){
        std::ifstream f(p); std::stringstream ss; ss<<f.rdbuf(); return ss.str();
    };
    std::string vs = slurp(vsPath), fs = slurp(fsPath);
    return ShaderProgram::fromSources(vs.c_str(), fs.c_str());
}

SpheresScene::SpheresScene(){
    sphere_ = std::make_unique<MeshModel>(
        SPHERE_VERTS, SPHERE_VERT_COUNT*3,
        SPHERE_NORMS, SPHERE_VERT_COUNT*3,
        SPHERE_IDX,   SPHERE_IDX_COUNT
    );
    spConst_  = loadProg("assets/shaders_basic/vs_posnorm_basic.glsl", "assets/shaders_basic/fs_constant_basic.glsl");
    spLambert_= loadProg("assets/shaders_basic/vs_posnorm_basic.glsl", "assets/shaders_basic/fs_lambert_basic.glsl");
    spPhong_  = loadProg("assets/shaders_basic/vs_posnorm_basic.glsl", "assets/shaders_basic/fs_phong_basic.glsl");
    spBlinn_  = loadProg("assets/shaders_basic/vs_posnorm_basic.glsl", "assets/shaders_basic/fs_blinn_basic.glsl");
}

void SpheresScene::update(float){}

void SpheresScene::render(const glm::mat4& V, const glm::mat4& P){
    // rozložení 2x2
    glm::mat4 M1 = glm::translate(glm::mat4(1.f), glm::vec3(-1.5f, 0.f, 0.f));
    glm::mat4 M2 = glm::translate(glm::mat4(1.f), glm::vec3( 1.5f, 0.f, 0.f));
    glm::mat4 M3 = glm::translate(glm::mat4(1.f), glm::vec3( 0.f, 0.f,-1.5f));
    glm::mat4 M4 = glm::translate(glm::mat4(1.f), glm::vec3( 0.f, 0.f, 1.5f));

    auto draw = [&](ShaderProgram& sp, const glm::mat4& M,
                    const glm::vec3& Ka, const glm::vec3& Kd,
                    const glm::vec3& Ks, float shininess){
        sp.use();
        sp.setUniform("uModel", M);
        sp.setUniform("uView",  V);
        sp.setUniform("uProj",  P);
        sp.setUniform("uLightPos", lightPos_);
        sp.setUniform("uViewPos",  camPos_);
        sp.setUniform("uKa", Ka);
        sp.setUniform("uKd", Kd);
        sp.setUniform("uKs", Ks);
        sp.setUniform("uShininess", shininess);
        sphere_->draw();
    };

    // Constant (uKd používáme jako barvu)
    spConst_.use();
    spConst_.setUniform("uModel", M1);
    spConst_.setUniform("uView", V);
    spConst_.setUniform("uProj", P);
    spConst_.setUniform("uKd", glm::vec3(0.9f,0.2f,0.2f));
    sphere_->draw();

    // Lambert
    draw(spLambert_, M2, {0.05f}, {0.3f,0.8f,0.3f}, {0,0,0}, 1.f);

    // Phong
    draw(spPhong_,   M3, {0.05f}, {0.8f,0.6f,0.3f}, {0.8f}, 32.f);

    // Blinn
    draw(spBlinn_,   M4, {0.05f}, {0.3f,0.5f,0.9f}, {0.8f}, 32.f);
}

// --- doplněno: viewPos z kamery (pro Phong/Blinn)
void SpheresScene::setViewPos(const glm::vec3& p){
    viewPos_ = p;
}
