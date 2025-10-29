#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <memory>

// GLEW před GLFW
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// MODELY (pozice + normála, 6 floatů na vrchol)
#include "Models/plain.h"        // glDrawArrays(..., 6)       — „rovina“ (2 trojúhelníky)
#include "Models/sphere.h"       // glDrawArrays(..., 2880)    — koule (smooth)
#include "Models/suzi_smooth.h"  // glDrawArrays(..., 2904)    — opička (smooth)

// --- util: kontrola GL chyb (volitelné)
static void glCheck(const char* where){
  GLenum e = glGetError();
  if(e!=GL_NO_ERROR){ std::fprintf(stderr,"[GL] error %x at %s\n", e, where); }
}

static void error_callback(int, const char* d){ std::fprintf(stderr, "GLFW error: %s\n", d); }

// --- kompilace/link shaderů
static GLuint compile(GLenum type, const char* src){
  GLuint s = glCreateShader(type);
  glShaderSource(s, 1, &src, nullptr);
  glCompileShader(s);
  GLint ok = GL_FALSE; glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
  if(!ok){
    GLint len=0; glGetShaderiv(s, GL_INFO_LOG_LENGTH, &len);
    std::string log(len, '\0'); glGetShaderInfoLog(s, len, nullptr, log.data());
    std::fprintf(stderr,"Shader compile error:\n%s\n", log.c_str());
  }
  return s;
}
static GLuint link(GLuint vs, GLuint fs){
  GLuint p = glCreateProgram();
  glAttachShader(p, vs); glAttachShader(p, fs);
  glLinkProgram(p);
  GLint ok = GL_FALSE; glGetProgramiv(p, GL_LINK_STATUS, &ok);
  if(!ok){
    GLint len=0; glGetProgramiv(p, GL_INFO_LOG_LENGTH, &len);
    std::string log(len, '\0'); glGetProgramInfoLog(p, len, nullptr, log.data());
    std::fprintf(stderr,"Program link error:\n%s\n", log.c_str());
  }
  return p;
}

// --- mini Shader wrapper (drží si lokace uniformů)
struct Shader {
  GLuint id = 0;
  GLint  uModel = -1, uView = -1, uProj = -1, uColor = -1;
  void use() const { glUseProgram(id); }
};

// --- shadery
static const char* VS_PN = R"GLSL(
#version 330 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aNormal;
uniform mat4 uModel, uView, uProj;
out vec3 vNormal;
void main(){
    vNormal = aNormal; // (pro jednoduchost v objekt. prostoru)
    gl_Position = uProj * uView * uModel * vec4(aPos, 1.0);
}
)GLSL";

static const char* FS_NORMALCOLOR = R"GLSL(
#version 330 core
in vec3 vNormal;
out vec4 fragColor;
void main(){
    vec3 n = normalize(vNormal);
    fragColor = vec4(n*0.5+0.5, 1.0); // map [-1,1] -> [0,1]
}
)GLSL";

static const char* FS_FLATCOLOR = R"GLSL(
#version 330 core
uniform vec3 uColor;
out vec4 fragColor;
void main(){ fragColor = vec4(uColor, 1.0); }
)GLSL";

// --- Mesh: VAO/VBO + počet vrcholů
struct Mesh {
  GLuint vao=0, vbo=0;
  GLsizei count=0; // počet vrcholů
  void draw() const {
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, count);
  }
};

template <size_t N>
Mesh makeMeshPN(const float (&data)[N]){
  Mesh m{};
  const GLsizeiptr BYTES = sizeof(data);
  const GLsizei floats = (GLsizei)(N);
  const GLsizei stride = 6; // pos(3)+normal(3)
  m.count = floats / stride;

  glGenVertexArrays(1, &m.vao);
  glGenBuffers(1, &m.vbo);

  glBindVertexArray(m.vao);
  glBindBuffer(GL_ARRAY_BUFFER, m.vbo);
  glBufferData(GL_ARRAY_BUFFER, BYTES, data, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));

  glBindVertexArray(0);
  return m;
}

// --- Kompozitní uzel scény (Composite)
struct Node {
  Mesh* mesh = nullptr;         // co kreslím (volitelné)
  Shader* shader = nullptr;     // jak kreslím (volitelné)
  glm::mat4 local = glm::mat4(1);
  std::vector<std::unique_ptr<Node>> children;

  void draw(const glm::mat4& parent, const glm::mat4& view, const glm::mat4& proj){
    glm::mat4 M = parent * local;
    if(mesh && shader){
      shader->use();
      if(shader->uModel!=-1) glUniformMatrix4fv(shader->uModel, 1, GL_FALSE, glm::value_ptr(M));
      if(shader->uView !=-1) glUniformMatrix4fv(shader->uView , 1, GL_FALSE, glm::value_ptr(view));
      if(shader->uProj !=-1) glUniformMatrix4fv(shader->uProj , 1, GL_FALSE, glm::value_ptr(proj));
      mesh->draw();
    }
    for(auto& ch : children) ch->draw(M, view, proj);
  }
};

int main(){
  glfwSetErrorCallback(error_callback);
  if(!glfwInit()){ std::fprintf(stderr,"ERROR: could not start GLFW3\n"); return EXIT_FAILURE; }

  // Core profile 3.3+ (doporučeno pro moderní OpenGL / cv02):
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* win = glfwCreateWindow(1200, 800, "ZPG — složitější modely", nullptr, nullptr);
  if(!win){ glfwTerminate(); return EXIT_FAILURE; }
  glfwMakeContextCurrent(win);
  glfwSwapInterval(1);

  glewExperimental = GL_TRUE;
  if(glewInit()!=GLEW_OK){ std::fprintf(stderr,"ERROR: glewInit()\n"); return EXIT_FAILURE; }
  glGetError(); // GLEW po sobě často zanechá GL_INVALID_ENUM

  // Info
  std::printf("OpenGL %s | GLSL %s\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));

  // --- Depth test
  glEnable(GL_DEPTH_TEST);

  // --- Shadery
  GLuint vs = compile(GL_VERTEX_SHADER, VS_PN);
  GLuint fsN = compile(GL_FRAGMENT_SHADER, FS_NORMALCOLOR);
  GLuint fsC = compile(GL_FRAGMENT_SHADER, FS_FLATCOLOR);

  Shader shNormal{ link(vs, fsN) };
  Shader shColor { link(vs, fsC) };

  auto fetchUniforms = [](Shader& s){
    s.uModel = glGetUniformLocation(s.id, "uModel");
    s.uView  = glGetUniformLocation(s.id, "uView");
    s.uProj  = glGetUniformLocation(s.id, "uProj");
    s.uColor = glGetUniformLocation(s.id, "uColor"); // u normal shaderu bude -1, to je OK
  };
  fetchUniforms(shNormal);
  fetchUniforms(shColor);

  // --- Meshy (pozice+normála, stride 6)
  Mesh ground = makeMeshPN(plain);        // 6 vrcholů
  Mesh ball   = makeMeshPN(sphere);       // 2880 vrcholů
  Mesh suzi   = makeMeshPN(suziSmooth);   // 2904 vrcholů

  // --- Projekce + kamera (pohled)
  int w, h; glfwGetFramebufferSize(win, &w, &h);
  glViewport(0, 0, w, h);
  glm::mat4 P = glm::perspective(glm::radians(60.f), float(w)/float(h), 0.1f, 100.f);
  glm::mat4 V = glm::translate(glm::mat4(1.f), glm::vec3(0, -0.3f, -4.0f)); // kamera kouká po -Z

  // --- Scéna (kompozit)
  auto root = std::make_unique<Node>();

  // Zem (plain): zvětšíme a položíme do Y= -1
  auto nGround = std::make_unique<Node>();
  nGround->mesh   = &ground;
  nGround->shader = &shNormal; // normála jako barva
  nGround->local  = glm::translate(glm::mat4(1), glm::vec3(0,-1,0)) * glm::scale(glm::mat4(1), glm::vec3(5,1,5));
  root->children.push_back(std::move(nGround));

  // Koule vlevo
  auto nBall = std::make_unique<Node>();
  nBall->mesh   = &ball;
  nBall->shader = &shNormal;
  nBall->local  = glm::translate(glm::mat4(1), glm::vec3(-1.2f, 0.0f, 0.0f));
  root->children.push_back(std::move(nBall));

  // Suzi vpravo (plná barva, točí se)
  auto nSuzi = std::make_unique<Node>();
  Node* suziPtr = nSuzi.get(); // abychom aktualizovali rotaci
  nSuzi->mesh   = &suzi;
  nSuzi->shader = &shColor;
  nSuzi->local  = glm::translate(glm::mat4(1), glm::vec3(1.2f, 0.0f, 0.0f)) * glm::scale(glm::mat4(1), glm::vec3(0.7f));
  root->children.push_back(std::move(nSuzi));

  float angle = 0.f;
  while(!glfwWindowShouldClose(win)){
    glfwPollEvents();
    angle += 0.6f * 0.016f; // ~0.6 rad/s

    // suzi rotace + barva shaderu
    shColor.use();
    if(shColor.uColor!=-1) glUniform3f(shColor.uColor, 0.95f, 0.35f, 0.15f);
    suziPtr->local = glm::translate(glm::mat4(1), glm::vec3(1.2f, 0.0f, 0.0f))
                   * glm::rotate(glm::mat4(1), angle, glm::vec3(0,1,0))
                   * glm::scale(glm::mat4(1), glm::vec3(0.7f));

    glClearColor(0.06f, 0.07f, 0.09f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    root->draw(glm::mat4(1), V, P);

    glfwSwapBuffers(win);
  }

  glfwTerminate();
  return EXIT_SUCCESS;
}
