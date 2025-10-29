# ==== fix_step2_compile.sh ====
set -euo pipefail

# 1) Scene.h — přidej hook na viewPos (pokud tam ještě není)
if ! grep -q "setViewPos" src/scene/Scene.h; then
  awk '{
    print
  } /virtual void render/ && !added {
    print "    // pro Phong/Blinn: kamera -> uViewPos"
    print "    virtual void setViewPos(const glm::vec3&) {}"
    added=1
  }' src/scene/Scene.h > tmp && mv tmp src/scene/Scene.h
  echo "[Scene.h] doplněn setViewPos(..)"
fi

# 2) SpheresScene.h — kompletní bezpečná verze
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

# 3) SpheresScene.cpp — bezpečná verze (fstream/sstream, sizeof počty, glm::vec3(..))
cat > src/scene/SpheresScene.cpp <<'EOF'
#include "scene/SpheresScene.h"
#include <fstream>
#include <sstream>
#include <string>
#include <glm/gtc/matrix_transform.hpp>

// přizpůsob se libovolným jménům polí v meshes/sphere.h
#include "meshes/sphere.h"

// Pomocné makra: vyber nejpravděpodobnější názvy polí.
// Pokud tvůj sphere.h používá jiné názvy, jen je tady přepiš.
#ifndef SP_VERTS
#  ifdef SPHERE_VERTS
#    define SP_VERTS SPHERE_VERTS
#  elif defined(sphere_vertices)
#    define SP_VERTS sphere_vertices
#  elif defined(vertices)
#    define SP_VERTS vertices
#  else
#    define SP_VERTS SPHERE_VERTS /* kdyby nic neprošlo, necháme to spadnout na linku */
#  endif
#endif

#ifndef SP_NORMS
#  ifdef SPHERE_NORMS
#    define SP_NORMS SPHERE_NORMS
#  elif defined(sphere_normals)
#    define SP_NORMS sphere_normals
#  elif defined(normals)
#    define SP_NORMS normals
#  else
#    define SP_NORMS SPHERE_NORMS
#  endif
#endif

#ifndef SP_IDX
#  ifdef SPHERE_IDX
#    define SP_IDX SPHERE_IDX
#  elif defined(sphere_indices)
#    define SP_IDX sphere_indices
#  elif defined(indices)
#    define SP_IDX indices
#  else
#    define SP_IDX SPHERE_IDX
#  endif
#endif

static std::string slurp(const char* p){
    std::ifstream f(p);
    std::stringstream ss; ss << f.rdbuf();
    return ss.str();
}

static ShaderProgram loadProg(const char* vsPath, const char* fsPath){
    std::string vs = slurp(vsPath);
    std::string fs = slurp(fsPath);
    return ShaderProgram::fromSources(vs.c_str(), fs.c_str());
}

SpheresScene::SpheresScene(){
    // spočti počty prvků podle velikosti pole v bytech
    const size_t posCount  = sizeof(SP_VERTS) / sizeof(float);
    const size_t normCount = sizeof(SP_NORMS) / sizeof(float);
    const size_t idxCount  = sizeof(SP_IDX)   / sizeof(unsigned);

    sphere_ = std::make_unique<MeshModel>(
        SP_VERTS,  posCount,
        SP_NORMS,  normCount,
        SP_IDX,    idxCount
    );

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

    auto draw=[&](ShaderProgram& sp, const mat4& M,
                  vec3 Ka, vec3 Kd, vec3 Ks, float shin){
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

    // Constant (uKd jako barva)
    spConst_.use();
    spConst_.setUniform("uModel", M1);
    spConst_.setUniform("uView",  V);
    spConst_.setUniform("uProj",  P);
    spConst_.setUniform("uKd", vec3(0.9f,0.25f,0.2f));
    sphere_->draw();

    // Lambert / Phong / Blinn (pozor na GLM 1.0: použij vec3(0.05f))
    draw(spLambert_, M2, vec3(0.05f), vec3(0.3f,0.8f,0.3f), vec3(0.0f),  1.f);
    draw(spPhong_,   M3, vec3(0.05f), vec3(0.8f,0.6f,0.3f), vec3(0.8f), 32.f);
    draw(spBlinn_,   M4, vec3(0.05f), vec3(0.3f,0.5f,0.9f), vec3(0.8f), 32.f);
}

void SpheresScene::setViewPos(const glm::vec3& p){
    viewPos_ = p;
}
EOF

# 4) main.cpp — doplň kameru, dt a výpočet V,P + zavolej setViewPos()
# 4a) include kamery (pokud chybí)
grep -q '#include "gfx/Camera.h"' src/app/main.cpp || \
sed -i '1i #include "gfx/Camera.h"' src/app/main.cpp

# 4b) po vytvoření okna přidej per-frame kameru (WSAD + RMB) a výpočet V,P
#     vložíme po první occurenci "glfwPollEvents();" v render smyčce
awk '
/const glm::mat4 I\(1\.f\);/ && !ins_cam {
  print;
  print "    // --- Camera state ---";
  print "    static Camera cam({0.f,1.6f,4.f}, -90.f, 0.f);";
  print "    static bool rmbDown=false; static double lastX=0.0, lastY=0.0;";
  ins_cam=1; next
}
{ print }' src/app/main.cpp > tmp && mv tmp src/app/main.cpp

awk '
/glfwPollEvents\(\);/ && !ins_block {
  print;
  print "            // --- per-frame input + V,P ---";
  print "            if (glfwGetMouseButton(w, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS){";
  print "                double x,y; glfwGetCursorPos(w,&x,&y);";
  print "                if(!rmbDown){ rmbDown=true; lastX=x; lastY=y; }";
  print "                double dx=x-lastX, dy=y-lastY; lastX=x; lastY=y;";
  print "                cam.processMouse((float)dx,(float)-dy,true);";
  print "            } else rmbDown=false;";
  print "            float dt = 1.0f/60.0f;";
  print "            bool fwd=glfwGetKey(w,GLFW_KEY_W)==GLFW_PRESS, back=glfwGetKey(w,GLFW_KEY_S)==GLFW_PRESS;";
  print "            bool left=glfwGetKey(w,GLFW_KEY_A)==GLFW_PRESS, right=glfwGetKey(w,GLFW_KEY_D)==GLFW_PRESS;";
  print "            cam.processKeyboard(fwd,back,left,right,dt);";
  print "            int ww,hh; glfwGetFramebufferSize(w,&ww,&hh); float aspect=(float)ww/(float)hh;";
  print "            glm::mat4 V = cam.view(); glm::mat4 P = cam.proj(60.f, aspect, 0.1f, 200.f);";
  print "            s->setViewPos(cam.position());";
  ins_block=1; next
}
{ print }' src/app/main.cpp > tmp && mv tmp src/app/main.cpp

# 4c) nahrad s->update(I) / s->render(I,I) na dt/V,P
sed -i 's/s->update(0.f);/s->update(dt);/g' src/app/main.cpp
sed -i 's/s->render(I, I);/s->render(V, P);/g' src/app/main.cpp

echo "✅ Opraveno. Teď přegeneruj build:"
echo "   cd cmake-build-debug && cmake --build . -j || (cd .. && mkdir -p cmake-build-debug && cd cmake-build-debug && cmake -DCMAKE_BUILD_TYPE=Debug .. && cmake --build . -j)"
