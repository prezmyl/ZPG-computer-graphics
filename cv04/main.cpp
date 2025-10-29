#include <cstdio>
#include <cstdlib>
#include <memory>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "ShaderProgram.h"
#include "PNArrayModel.h"
#include "DrawableObject.h"
#include "Transform.h"
#include "TransformChain.h"

// data (pos+normal, stride 6)
#include "Models/plain.h"
#include "Models/sphere.h"
#include "Models/suzi_smooth.h"

#include "Scene.h"
#include "Scenes.h"

// global pro key-callbacku
static SceneManager* gMgr = nullptr;
static void key_cb(GLFWwindow* w, int key, int, int action, int){
    if (action != GLFW_PRESS) return;
    if (key >= GLFW_KEY_1 && key <= GLFW_KEY_9) {
        gMgr->switchTo(key - GLFW_KEY_1); // 1→scéna 0, 2→scéna 1
    }
}


// --- shadery
static const char* VS_PN = R"GLSL(
#version 330 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aNormal;
uniform mat4 uModel, uView, uProj;
out vec3 vNormal;
void main(){
    vNormal = aNormal;
    gl_Position = uProj * uView * uModel * vec4(aPos, 1.0);
}
)GLSL";

static const char* FS_NORMALCOLOR = R"GLSL(
#version 330 core
in vec3 vNormal;
out vec4 fragColor;
void main(){
    vec3 n = normalize(vNormal);
    fragColor = vec4(n*0.5+0.5, 1.0);
}
)GLSL";

static const char* FS_FLATCOLOR = R"GLSL(
#version 330 core
uniform vec3 uColor;
out vec4 fragColor;
void main(){ fragColor = vec4(uColor, 1.0); }
)GLSL";

int main(){
    if(!glfwInit()){ std::fprintf(stderr,"ERROR: GLFW init\n"); return EXIT_FAILURE; }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* w = glfwCreateWindow(1200, 800, "ZPG – minimal split", nullptr, nullptr);
    if(!w){ glfwTerminate(); return EXIT_FAILURE; }
    glfwMakeContextCurrent(w);
    glfwSwapInterval(1);

    glewExperimental = GL_TRUE;
    if(glewInit()!=GLEW_OK){ std::fprintf(stderr,"ERROR: glewInit\n"); return EXIT_FAILURE; }
    glGetError(); // glew cleanup

    glEnable(GL_DEPTH_TEST);

    // --- programs ---
    ShaderProgram progNormal = ShaderProgram::fromSources(VS_PN, FS_NORMALCOLOR);
    ShaderProgram progColor  = ShaderProgram::fromSources(VS_PN, FS_FLATCOLOR);

    // --- models (pouze koule pro scénu 2) ---
    PNArrayModel sphereModel(sphere);

    // --- cameras + projection ---
    int fbw, fbh; glfwGetFramebufferSize(w, &fbw, &fbh);
    glViewport(0,0,fbw,fbh);
    glm::mat4 P = glm::perspective(glm::radians(60.f), float(fbw)/float(fbh), 0.1f, 100.f);
    glm::mat4 V = glm::translate(glm::mat4(1.f), glm::vec3(0, -0.3f, -4.0f));

    // --- scenes + switching scenes 1/2 ---
    SceneManager mgr;
    mgr.add(std::make_unique<RotatingTriangleScene>(&progColor));               // key 1
    mgr.add(std::make_unique<FourSpheresScene>(&progNormal, &sphereModel));     // key 2
    gMgr = &mgr;
    glfwSetKeyCallback(w, key_cb);
/*
    // programy
    ShaderProgram progNormal = ShaderProgram::fromSources(VS_PN, FS_NORMALCOLOR);
    ShaderProgram progColor  = ShaderProgram::fromSources(VS_PN, FS_FLATCOLOR);

    // modely
    PNArrayModel ground(plain);
    PNArrayModel sphereModel(sphere);
    PNArrayModel suziModel(suziSmooth);

    // kamera + projekce
    int fbw, fbh; glfwGetFramebufferSize(w, &fbw, &fbh);
    glViewport(0,0,fbw,fbh);
    glm::mat4 P = glm::perspective(glm::radians(60.f), float(fbw)/float(fbh), 0.1f, 100.f);
    glm::mat4 V = glm::translate(glm::mat4(1.f), glm::vec3(0, -0.3f, -4.0f));

    // --- objekty (DrawableObject)

    // zem
    auto groundT = std::make_shared<TransformChain>();
    groundT->add(std::make_shared<Translate>(glm::vec3(0,-1,0)));
    groundT->add(std::make_shared<Scale>(glm::vec3(5,1,5)));

    DrawableObject groundObj;
    groundObj.model = &ground;
    groundObj.program = &progNormal;
    groundObj.transform = groundT;

    // koule vlevo
    auto ballT = std::make_shared<TransformChain>();
    ballT->add(std::make_shared<Translate>(glm::vec3(-1.2f, 0, 0)));
    DrawableObject ballObj { &sphereModel, &progNormal, ballT };

    // suzi vpravo – tocit
    auto suziT = std::make_shared<TransformChain>();
    auto spin  = std::make_shared<Rotate>(0.f, glm::vec3(0,1,0)); // menit uhel
    suziT->add(std::make_shared<Translate>(glm::vec3(1.2f, 0, 0)));
    suziT->add(spin);
    suziT->add(std::make_shared<Scale>(glm::vec3(0.7f)));

    DrawableObject suziObj { &suziModel, &progColor, suziT };

    std::vector<DrawableObject*> scene = { &groundObj, &ballObj, &suziObj };
*/
    while(!glfwWindowShouldClose(w)){
        glfwPollEvents();
        // color for FS_FLATCOLOR (scéna 1 uses progColor)
        progColor.use();
        progColor.setUniform("uColor", glm::vec3(0.95f, 0.35f, 0.15f));

        glClearColor(0.06f,0.07f,0.09f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (auto* s = mgr.active()) {
            s->update(0.f);       // trinagle si bere glfwGetTime()
            s->render(V, P);
        }

        /* animation suzi
        spin->deg = (float)glfwGetTime() * 50.f;
        progColor.use();
        progColor.setUniform("uColor", glm::vec3(0.95f, 0.35f, 0.15f));

        glClearColor(0.06f,0.07f,0.09f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for(auto* o : scene) o->draw(V, P);
*/
        glfwSwapBuffers(w);
    }

    glfwTerminate();
    return EXIT_SUCCESS;
}
