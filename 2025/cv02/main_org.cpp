#include <cstdio>
#include <cstdlib>
#include <string>

// GLEW musí být před GLFW
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Models/sphere.h"


static void error_callback(int, const char* desc){ std::fprintf(stderr, "GLFW error: %s\n", desc); }


static const char* vsrc =
"#version 330\n"
"layout(location=0) in vec3 vp;"
"void main(){ gl_Position = vec4(vp, 1.0); }";

static const char* fsrc =
"#version 330\n"
"out vec4 fragColor;"
"void main(){ fragColor = vec4(0.5, 0.0, 0.5, 1.0); }";

int main(){
  glfwSetErrorCallback(error_callback);
  if(!glfwInit()){ std::fprintf(stderr,"ERROR: could not start GLFW3\n"); return EXIT_FAILURE; }

  // Volitelně vynutit core profile 3.3+ (cv02 doporučuje)
  // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* win = glfwCreateWindow(800, 600, "ZPG", nullptr, nullptr);
  if(!win){ glfwTerminate(); return EXIT_FAILURE; }
  glfwMakeContextCurrent(win);
  glfwSwapInterval(1);

  glewExperimental = GL_TRUE;
  if(glewInit()!=GLEW_OK){ std::fprintf(stderr,"ERROR: glewInit()\n"); return EXIT_FAILURE; }
  // „GLEW init“ může zanechat GL_ERROR, pro jistotu zahodíme:
  glGetError();

  // Info
  std::printf("OpenGL Version: %s\n", glGetString(GL_VERSION));
  std::printf("Using GLEW %s\n", glewGetString(GLEW_VERSION));
  std::printf("Vendor %s\n", glGetString(GL_VENDOR));
  std::printf("Renderer %s\n", glGetString(GL_RENDERER));
  std::printf("GLSL %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

  // Data trojúhelníku
  float points[] = {
     0.0f,  0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f
  };

  const float a[] ={
    -.5f, -.5f, .5f,  0, 0, 1,
    -.5f, .5f, .5f,  0, 0, 1,
      .5f, .5f, .5f,  0, 0, 1 };

  // VBO
  GLuint vbo=0; glGenBuffers(1,&vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

  // VAO
  GLuint vao=0; glGenVertexArrays(1,&vao);
  glBindVertexArray(vao);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);

  // Shadery a program
  auto compile = [](GLenum type, const char* src)->GLuint{
    GLuint s = glCreateShader(type);
    glShaderSource(s,1,&src,nullptr);
    glCompileShader(s);
    GLint ok=GL_FALSE; glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if(!ok){
      GLint n=0; glGetShaderiv(s, GL_INFO_LOG_LENGTH, &n);
      std::string log(n,'\0'); glGetShaderInfoLog(s,n,nullptr,log.data());
      std::fprintf(stderr,"Shader compile error: %s\n", log.c_str());
      std::exit(EXIT_FAILURE);
    }
    return s;
  };

  GLuint vs = compile(GL_VERTEX_SHADER, vsrc);
  GLuint fs = compile(GL_FRAGMENT_SHADER, fsrc);
  GLuint prog = glCreateProgram();
  glAttachShader(prog, vs);
  glAttachShader(prog, fs);
  glLinkProgram(prog);

  // Kontrola linku (viz cv02)
  GLint linked=GL_FALSE;
  glGetProgramiv(prog, GL_LINK_STATUS, &linked);
  if(!linked){
    GLint n=0; glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &n);
    std::string log(n,'\0'); glGetProgramInfoLog(prog,n,nullptr,log.data());
    std::fprintf(stderr,"Linker failure: %s\n", log.c_str());
    return EXIT_FAILURE;
  }

  // Viewport
  int w,h; glfwGetFramebufferSize(win,&w,&h);
  glViewport(0,0,w,h);

  // Loop
  while(!glfwWindowShouldClose(win)){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(prog);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glfwPollEvents();
    glfwSwapBuffers(win);
  }

  glfwDestroyWindow(win);
  glfwTerminate();
  return EXIT_SUCCESS;
}
