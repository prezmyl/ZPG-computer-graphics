# fix_cv04_du_build.sh
set -euo pipefail

# 0) Záloha
mkdir -p _backup
for f in src/models/MeshModel.h src/models/MeshModel.cpp src/scene/SpheresScene.h src/scene/SpheresScene.cpp src/app/main.cpp; do
  [[ -f "$f" ]] && cp "$f" "_backup/$(basename "$f").bak" || true
done

# 1) MeshModel (interleaved: px,py,pz, nx,ny,nz), kreslení přes glDrawArrays
cat > src/models/MeshModel.h <<'EOF'
#pragma once
#include <cstddef>
#include <GL/glew.h>

class MeshModel {
public:
    // interleaved: [px,py,pz, nx,ny,nz] * vertexCount
    MeshModel(const float* interleaved, size_t vertexCount, bool hasNormals=true, int strideFloats=6);
    MeshModel(const MeshModel&) = delete;
    MeshModel& operator=(const MeshModel&) = delete;
    MeshModel(MeshModel&&) noexcept;
    MeshModel& operator=(MeshModel&&) noexcept;
    ~MeshModel();
    void draw() const;
private:
    GLuint vao_=0, vbo_=0;
    GLsizei vertexCount_=0;
    GLsizei strideBytes_=0;
    bool hasNormals_=true;
};
EOF

cat > src/models/MeshModel.cpp <<'EOF'
#include "models/MeshModel.h"
#include <GL/glew.h>

MeshModel::MeshModel(const float* data, size_t vertexCount, bool hasNormals, int strideFloats)
: hasNormals_(hasNormals), vertexCount_((GLsizei)vertexCount), strideBytes_((GLsizei)strideFloats*sizeof(float)) {
    glGenVertexArrays(1, &vao_); glBindVertexArray(vao_);
    glGenBuffers(1, &vbo_); glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(vertexCount * (size_t)strideBytes_), data, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, strideBytes_, (void*)0);

    if (hasNormals_) {
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, strideBytes_, (void*)(3*sizeof(float)));
    }
    glBindVertexArray(0);
}

MeshModel::MeshModel(MeshModel&& o) noexcept {
    vao_=o.vao_; vbo_=o.vbo_; vertexCount_=o.vertexCount_; strideBytes_=o.strideBytes_; hasNormals_=o.hasNormals_;
    o.vao_=o.vbo_=0; o.vertexCount_=0; o.strideBytes_=0;
}
MeshModel& MeshModel::operator=(MeshModel&& o) noexcept {
    if (this!=&o){ this->~MeshModel(); vao_=o.vao_; vbo_=o.vbo_; vertexCount_=o.vertexCount_; strideBytes_=o.strideBytes_; hasNormals_=o.hasNormals_;
        o.vao_=o.vbo_=0; o.vertexCount_=0; o.strideBytes_=0; }
    return *this;
}
MeshModel::~MeshModel(){ if(vbo_) glDeleteBuffers(1,&vbo_); if(vao_) glDeleteVertexArrays(1,&vao_); }
void MeshModel::draw() const { glBindVertexArray(vao_); glDrawArrays(GL_TRIANGLES, 0, vertexCount_); glBindVertexArray(0); }
EOF

# 2) SpheresScene – čti interleavované meshes/sphere.h (pole sphere[]), načítej shadery ze souborů
cat > src/scene/SpheresScene.h <<'EOF'
#pragma once
#include "scene/Scene.h"
#include "gfx/ShaderProgram.h"
#include "models/MeshModel.h"
#include <memory>
#include <glm/glm.hpp>

class SpheresScene : public Scene {
public:
    SpheresScene();
    void update(float dt) override;
    void render(const glm::mat4& V, const glm::mat4& P) override;
    void setViewPos(const glm::vec3&) override;
private:
    std::unique_ptr<MeshModel> sphere_;
    ShaderProgram spConst_, spLambert_, spPhong_, spBlinn_;
    glm::vec3 lightPos_{0.f, 1.5f, 0.f};
    glm::vec3 viewPos_{0.f, 1.6f, 4.f};
};
EOF

cat > src/scene/SpheresScene.cpp <<'EOF'
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
EOF

# 3) main.cpp – srovnat include a vložit setViewPos těsně před update(dt)
# 3a) přidej bezpečně include na kameru a SpheresScene (pokud chybí)
grep -q '#include "gfx/Camera.h"' src/app/main.cpp || sed -i '1i #include "gfx/Camera.h"' src/app/main.cpp
grep -q '#include "scene/SpheresScene.h"' src/app/main.cpp || sed -i 's|#include "scene/Scenes.h"|#include "scene/Scenes.h"\n#include "scene/SpheresScene.h"|g' src/app/main.cpp

# 3b) smaž všechny dřívější setViewPos(cam.position()) (špatné umístění)
sed -i '/setViewPos\s*(\s*cam\.position\s*\(\s*\)\s*)\s*;$/d' src/app/main.cpp

# 3c) vlož správně – těsně před první s->update(
awk '{
  if(!done && $0 ~ /s->update\s*\(/){
    print "            s->setViewPos(cam.position());"
    done=1
  }
  print
}' src/app/main.cpp > src/app/.main.tmp && mv src/app/.main.tmp src/app/main.cpp

echo "✅ Patch hotový. Teď build:"
echo "   cd cmake-build-debug && cmake --build . -j || (mkdir -p cmake-build-debug && cd cmake-build-debug && cmake -DCMAKE_BUILD_TYPE=Debug .. && cmake --build . -j)"
