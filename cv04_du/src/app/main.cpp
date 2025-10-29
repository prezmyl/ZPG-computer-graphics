#include "gfx/Camera.h"
#include <cstdio>
#include <cstdlib>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "gfx/ShaderProgram.h"
#include "scene/Scene.h"
#include "scene/Scenes.h"
#include "scene/SpheresScene.h"


// GLSL
static const char* VS_POS = R"GLSL(
#version 330 core
layout(location=0) in vec3 aPos;
uniform mat4 uModel, uView, uProj;
void main(){ gl_Position = uProj * uView * uModel * vec4(aPos, 1.0); }
)GLSL";

static const char* FS_FLAT = R"GLSL(
#version 330 core
uniform vec3 uColor;
out vec4 fragColor;
void main(){ fragColor = vec4(uColor, 1.0); }
)GLSL";

// prepinani scen
static SceneManager* gMgr = nullptr;
static void key_cb(GLFWwindow*, int key, int, int action, int){
    if (action != GLFW_PRESS) return;
    int idx = -1;
    if (key >= GLFW_KEY_1 && key <= GLFW_KEY_9) idx = key - GLFW_KEY_1;
    else if (key >= GLFW_KEY_KP_1 && key <= GLFW_KEY_KP_9) idx = key - GLFW_KEY_KP_1;
    if (idx != -1 && gMgr){ gMgr->switchTo(idx); std::printf("Scene -> %d\n", idx); }
}

int main(){
    if(!glfwInit()){ std::fprintf(stderr,"GLFW init fail\n"); return EXIT_FAILURE; }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* w = glfwCreateWindow(1000,700,"ZPG – sceny bez kamery/projekce",nullptr,nullptr);
    if(!w){ glfwTerminate(); return EXIT_FAILURE; }
    glfwMakeContextCurrent(w);
    glfwSwapInterval(1);

    glewExperimental = GL_TRUE;
    if(glewInit()!=GLEW_OK){ std::fprintf(stderr,"GLEW init fail\n"); return EXIT_FAILURE; }
    glGetError();
    glEnable(GL_DEPTH_TEST);

    // program
    ShaderProgram prog = ShaderProgram::fromSources(VS_POS, FS_FLAT);

    // sceny
    SceneManager mgr;
    mgr.add(std::make_unique<RotatingTriangleScene>(&prog)); // 1
    mgr.add(std::make_unique<SpheresScene>());
    mgr.add(std::make_unique<EmptyScene>());                  // 2
    gMgr = &mgr;
    glfwSetKeyCallback(w, key_cb);


    const glm::mat4 I(1.f);
    // --- Camera state ---
    static Camera cam({0.f,1.6f,4.f}, -90.f, 0.f);
    static bool rmbDown=false; static double lastX=0.0, lastY=0.0;

    while(!glfwWindowShouldClose(w)){
        glfwPollEvents();
            // --- per-frame input + V,P ---
            if (glfwGetMouseButton(w, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS){
                double x,y; glfwGetCursorPos(w,&x,&y);
                if(!rmbDown){ rmbDown=true; lastX=x; lastY=y; }
                double dx=x-lastX, dy=y-lastY; lastX=x; lastY=y;
                cam.processMouse((float)dx,(float)-dy,true);
            } else rmbDown=false;
            float dt = 1.0f/60.0f;
            bool fwd=glfwGetKey(w,GLFW_KEY_W)==GLFW_PRESS, back=glfwGetKey(w,GLFW_KEY_S)==GLFW_PRESS;
            bool left=glfwGetKey(w,GLFW_KEY_A)==GLFW_PRESS, right=glfwGetKey(w,GLFW_KEY_D)==GLFW_PRESS;
            cam.processKeyboard(fwd,back,left,right,dt);
            int ww,hh; glfwGetFramebufferSize(w,&ww,&hh); float aspect=(float)ww/(float)hh;
            glm::mat4 V = cam.view(); glm::mat4 P = cam.proj(60.f, aspect, 0.1f, 200.f);


        // barva pro flat shader
        prog.use();
        prog.setUniform("uColor", glm::vec3(0.95f, 0.25f, 0.2f));

        glClearColor(0.06f,0.07f,0.09f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (auto* s = mgr.active()){

            s->setViewPos(cam.position());
            s->update(dt);
            s->render(V, P);

        }

        glfwSwapBuffers(w);
    }
    glfwTerminate();
    return EXIT_SUCCESS;
}
