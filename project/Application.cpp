//
// Created by pol0511 on 10/5/26.
//

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#undef GLAD_GL_IMPLEMENTATION

#include <GLFW/glfw3.h>

#include <cstdio>
#include <cstdlib>

#include "Application.h"
#include "Model.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "Translation.h"
#include "DrawableObject.h"

//static - original callbacks
static void error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

static void key_callback(GLFWwindow* window, int key,
                         int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static void window_size_callback(GLFWwindow* window,
                                 int width, int height)
{
    glViewport(0, 0, width, height);
}



void Application::createResources() {
    const float points[] = {

        // 1. trojúhelník
        // levý horní
        -0.5f,  0.5f, 0.0f,
         1.0f,  0.0f, 0.0f,

        // pravý horní
         0.5f,  0.5f, 0.0f,
         0.0f,  1.0f, 0.0f,

        // pravý dolní
         0.5f, -0.5f, 0.0f,
         0.0f,  0.0f, 1.0f,


        // 2. trojúhelník
        // levý horní
        -0.5f,  0.5f, 0.0f,
         1.0f,  0.0f, 0.0f,

        // pravý dolní
         0.5f, -0.5f, 0.0f,
         0.0f,  0.0f, 1.0f,

        // levý dolní
        -0.5f, -0.5f, 0.0f,
         1.0f,  1.0f, 0.0f
    };

    model = std::make_unique<Model>(points,6);

    Shader vertexShader(GL_VERTEX_SHADER,"shaders/basic.vert");
    Shader fragmentShader(GL_FRAGMENT_SHADER, "shaders/basic.frag");

    shaderProgram = std::make_unique<ShaderProgram>(vertexShader, fragmentShader);


    std::unique_ptr<Translation> move = std::make_unique<Translation>(Translation(glm::vec3(0.25f, 0.0f, 0.0f)));
    drawableObject = std::make_unique<DrawableObject>(model, shaderProgram), std::move(move));


}

Application::Application() = default;


bool Application::initialize() {
    // GLFW error callback
    glfwSetErrorCallback(error_callback);

    // Inicialization GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "GLFW initialization failed\n");
        return false;
    }


    // OpenGL 3.3 Core Profile
    glfwWindowHint(
        GLFW_CONTEXT_VERSION_MAJOR,
        3
    );

    glfwWindowHint(
        GLFW_CONTEXT_VERSION_MINOR,
        3
    );

    glfwWindowHint(
        GLFW_OPENGL_PROFILE,
        GLFW_OPENGL_CORE_PROFILE
    );

    // Window creation
    this->window = glfwCreateWindow(
        800,
        600,
        "ZPG - DU01 - square",
        nullptr,
        nullptr
    );

    if (!window)
    {
        fprintf(stderr, "Window creattion failed\n");
        return false;
    }


    // OpenGL context setup
    glfwMakeContextCurrent(window);

    // VSync
    glfwSwapInterval(1);


    // Init GLAD
    if (!gladLoadGL((GLADloadfunc) glfwGetProcAddress))
    {
        fprintf(
            stderr,
            "GLAD initialization failed\n"
        );

        return false;
    }


    // Callbacks
    glfwSetKeyCallback(
        window,
        key_callback
    );

    glfwSetWindowSizeCallback(
        window,
        window_size_callback
    );

    createResources();

    return true;
}

Application::~Application() {
    shaderProgram.reset();
    model.reset();
    if (window != nullptr) {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    glfwTerminate();
}

void Application::run() {
    while (!glfwWindowShouldClose(window))
    {
        // background color
        glClearColor(
            0.15f,
            0.15f,
            0.15f,
            1.0f
        );


        // cleaning framebuffer
        glClear(
            GL_COLOR_BUFFER_BIT |
            GL_DEPTH_BUFFER_BIT
        );

        // == using made oop interfaces ==


        // display result
        glfwSwapBuffers(window);


        // process events
        glfwPollEvents();
    }
}

