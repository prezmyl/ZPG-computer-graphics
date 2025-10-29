// main.cpp
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cstdio>
#include <cstdlib>
#include <string>

// ---------- Shadery (jednoduché, pozice + barva) ----------
static const char* VERT_SRC = R"GLSL(
#version 330 core
layout (location=0) in vec3 inPos;
layout (location=1) in vec3 inCol;

uniform mat4 MVP;

out vec3 vCol;

void main() {
    gl_Position = MVP * vec4(inPos, 1.0);
    vCol = inCol;
}
)GLSL";

static const char* FRAG_SRC = R"GLSL(
#version 330 core
in vec3 vCol;
out vec4 fragColor;
void main() {
    fragColor = vec4(vCol, 1.0);
}
)GLSL";

// ---------- Stav aplikace ----------
struct AppState {
    float angle = 0.0f;        // úhel rotace (rad)
    float speed = 1.0f;        // rychlost (rad/s)
    int dir = +1;              // směr rotace (+1 / -1)
    glm::vec3 axis = {0,0,1};  // aktuální osa rotace
    bool rotateAroundCorner = false; // rotace kolem rohu (P)
} g;

// ---------- Callbacky ----------
static void key_callback(GLFWwindow* w, int key, int sc, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(w, GLFW_TRUE); break;
            case GLFW_KEY_SPACE:  g.dir = -g.dir; break;             // přepni směr
            case GLFW_KEY_X:      g.axis = {1,0,0}; break;
            case GLFW_KEY_Y:      g.axis = {0,1,0}; break;
            case GLFW_KEY_Z:      g.axis = {0,0,1}; break;
            case GLFW_KEY_C:      g.rotateAroundCorner = !g.rotateAroundCorner; break;
            case GLFW_KEY_KP_ADD:
            case GLFW_KEY_EQUAL:  g.speed *= 1.25f; break;            // zrychli
            case GLFW_KEY_KP_SUBTRACT:
            case GLFW_KEY_MINUS:  g.speed *= 0.8f;  break;            // zpomal
            default: break;
        }
    }
}

static void framebuffer_size_callback(GLFWwindow*, int w, int h) {
    glViewport(0, 0, w, h);
}

// ---------- Pomůcky ----------
static GLuint mkShader(GLenum type, const char* src) {
    GLuint sh = glCreateShader(type);
    glShaderSource(sh, 1, &src, nullptr);
    glCompileShader(sh);
    GLint ok = GL_FALSE;
    glGetShaderiv(sh, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        GLint len = 0;
        glGetShaderiv(sh, GL_INFO_LOG_LENGTH, &len);
        std::string log(len, '\0');
        glGetShaderInfoLog(sh, len, nullptr, log.data());
        fprintf(stderr, "Shader compile error:\n%s\n", log.c_str());
        exit(EXIT_FAILURE);
    }
    return sh;
}

static GLuint mkProgram(const char* vs, const char* fs) {
    GLuint v = mkShader(GL_VERTEX_SHADER, vs);
    GLuint f = mkShader(GL_FRAGMENT_SHADER, fs);
    GLuint p = glCreateProgram();
    glAttachShader(p, v);
    glAttachShader(p, f);
    glLinkProgram(p);
    glDeleteShader(v);
    glDeleteShader(f);

    GLint ok = GL_FALSE;
    glGetProgramiv(p, GL_LINK_STATUS, &ok);
    if (!ok) {
        GLint len = 0;
        glGetProgramiv(p, GL_INFO_LOG_LENGTH, &len);
        std::string log(len, '\0');
        glGetProgramInfoLog(p, len, nullptr, log.data());
        fprintf(stderr, "Program link error:\n%s\n", log.c_str());
        exit(EXIT_FAILURE);
    }
    return p;
}

int main() {
    // ---------- GLFW init ----------
    if (!glfwInit()) { fprintf(stderr, "GLFW init failed\n"); return EXIT_FAILURE; }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* win = glfwCreateWindow(800, 600, "ZPG - cv01", nullptr, nullptr);
    if (!win) { fprintf(stderr, "Window create failed\n"); glfwTerminate(); return EXIT_FAILURE; }
    glfwMakeContextCurrent(win);
    glfwSwapInterval(1);

    glfwSetKeyCallback(win, key_callback);
    glfwSetFramebufferSizeCallback(win, framebuffer_size_callback);

    // ---------- GLEW init ----------
    glewExperimental = GL_TRUE;
    GLenum glewErr = glewInit();
    if (glewErr != GLEW_OK) {
        fprintf(stderr, "GLEW init error: %s\n", (const char*)glewGetErrorString(glewErr));
        return EXIT_FAILURE;
    }

    // Info (dobré pro debug)
    printf("OpenGL: %s\n", glGetString(GL_VERSION));
    printf("GLSL:   %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    printf("Vendor: %s\n", glGetString(GL_VENDOR));
    printf("Renderer: %s\n", glGetString(GL_RENDERER));

    // ---------- Geometrie: čtverec (2 trojúhelníky), pozice a barva ----------
    // Vrcholy v NDC ([-1,1]) kolem středu; poslední vrchol je žlutý.
    // Pořadí vrcholů:
    // 0: (-0.5, -0.5)  červená
    // 1: (-0.5,  0.5)  zelená
    // 2: ( 0.5,  0.5)  modrá
    // 3: ( 0.5, -0.5)  ŽLUTÁ  <-- požadavek
    const float vertices[] = {
        //    x      y     z      r     g     b
        -0.5f, -0.5f, 0,   1.0f, 0.0f, 0.0f, // 0
        -0.5f,  0.5f, 0,   0.0f, 1.0f, 0.0f, // 1
         0.5f,  0.5f, 0,   0.0f, 0.0f, 1.0f, // 2
         0.5f, -0.5f, 0,   1.0f, 1.0f, 0.0f  // 3  (žlutý)
    };
    const unsigned int indices[] = {
        0, 1, 2,  // první trojúhelník
        0, 2, 3   // druhý trojúhelník
    };

    GLuint vao=0, vbo=0, ebo=0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // layout(location=0) vec3 inPos
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
    // layout(location=1) vec3 inCol
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));

    glBindVertexArray(0); // (ponecháme EBO bindnuté s VAO uvnitř)

    // ---------- Program ----------
    GLuint prog = mkProgram(VERT_SRC, FRAG_SRC);
    GLint locMVP = glGetUniformLocation(prog, "MVP");

    // ---------- Stav scény ----------
    glClearColor(0.08f, 0.08f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    int w, h;
    glfwGetFramebufferSize(win, &w, &h);

    double last = glfwGetTime();

    while (!glfwWindowShouldClose(win)) {
        double now = glfwGetTime();
        float dt = float(now - last);
        last = now;

        g.angle += g.dir * g.speed * dt; // plynulá změna úhlu

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // --- matice ---
        glfwGetFramebufferSize(win, &w, &h);
        float aspect = (h>0) ? (float)w / (float)h : 1.0f;

        // Projekce + "kamera" (2D scéna, ale použijeme ortho/perspective pro demonstraci)
        glm::mat4 P = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 10.0f);
        glm::mat4 V = glm::lookAt(glm::vec3(0,0,2.0f), glm::vec3(0,0,0), glm::vec3(0,1,0));

        // Model: buď rotace kolem středu (0,0,0),
        // nebo kolem pravého-dolního rohu čtverce (0.5, -0.5, 0) – to je náš P.
        glm::mat4 M(1.0f);

        if (g.rotateAroundCorner) {
            glm::vec3 Piv(0.5f, -0.5f, 0.0f); // roh čtverce
            M = glm::translate(glm::mat4(1.0f), Piv)
              * glm::rotate(glm::mat4(1.0f), g.angle, glm::normalize(g.axis))
              * glm::translate(glm::mat4(1.0f), -Piv);
        } else {
            M = glm::rotate(glm::mat4(1.0f), g.angle, glm::normalize(g.axis));
        }

        glm::mat4 MVP = P * V * M;

        // --- vykreslení ---
        glUseProgram(prog);
        glUniformMatrix4fv(locMVP, 1, GL_FALSE, glm::value_ptr(MVP));

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(win);
        glfwPollEvents();
    }

    glDeleteProgram(prog);
    glDeleteBuffers(1, &ebo);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);

    glfwDestroyWindow(win);
    glfwTerminate();
    return 0;
}
