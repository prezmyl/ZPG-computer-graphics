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

#include "CompositeTransformation.h"
#include "Model.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "Translation.h"
#include "DrawableObject.h"
#include "Rotation.h"
#include "Scale.h"
#include "Scene.h"
#include <glm/gtc/matrix_transform.hpp>

#include "Models/bushes.h"
#include "Models/tree.h"

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

    model = std::make_unique<Model>(bushes, static_cast<GLsizei>(sizeof(bushes) / (6 * sizeof(float))));
    treeModel = std::make_unique<Model>(tree, static_cast<GLsizei>(sizeof(tree) / (6 * sizeof(float))));

    Shader vertexShader(GL_VERTEX_SHADER,"shaders/normal.vert");
    Shader fragmentShader(GL_FRAGMENT_SHADER, "shaders/normal.frag");

    shaderProgram = std::make_unique<ShaderProgram>(vertexShader, fragmentShader);



  /*  task 4
   *  auto composite1 = std::make_unique<CompositeTransformation>();
    //composite1->addTransformation(std::make_unique<Scale>(glm::vec3(0.5f, 0.5f, 1.0f)));
    composite1->addTransformation(std::make_unique<Rotation>(glm::vec3(0.0f, 0.0f, 1.0f), glm::radians(45.0f)));
    composite1->addTransformation(std::make_unique<Translation>(glm::vec3(0.4f, 0.0f, 0.0f)));


    auto object1 = std::make_unique<DrawableObject>(model.get(), shaderProgram.get(), std::move(composite1));

    auto composite2 = std::make_unique<CompositeTransformation>();
    //composite2->addTransformation(std::make_unique<Scale>(glm::vec3(0.5f, 0.5f, 1.0f)));
    composite2->addTransformation(std::make_unique<Translation>(glm::vec3(-0.4f, 0.0f, 0.0f)));
    composite2->addTransformation(std::make_unique<Rotation>(glm::vec3(0.0f, 0.0f, 1.0f), glm::radians(-45.0f)));


    auto object2 = std::make_unique<DrawableObject>(model.get(), shaderProgram.get(), std::move(composite2));

    scene = std::make_unique<Scene>();
    scene->addObject(std::move(object1));
    scene->addObject(std::move(object2)); */
    // lambda func temporary for transforamtion
    scene = std::make_unique<Scene>();
    auto addInstance = [&](Model* mesh, glm::vec3 position, float size) {
        auto transform = std::make_unique<CompositeTransformation>();

        transform->addTransformation(
            std::make_unique<Scale>(glm::vec3(size))
        );

        transform->addTransformation(
            std::make_unique<Translation>(position)
        );

        scene->addObject(std::make_unique<DrawableObject>(
            mesh,
            shaderProgram.get(),
            std::move(transform)
        ));
    };

    addInstance(treeModel.get(), {-1.0f, 0.0f, 0.0f}, 1.0f);
    addInstance(treeModel.get(), { 1.0f, 0.0f, 0.0f}, 0.8f);

    addInstance(model.get(), {-0.5f, 0.0f, 1.0f}, 3.0f);
    addInstance(model.get(), { 0.5f, 0.0f, 1.0f}, 2.5f);



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
        "ZPG - DU02",
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

    glEnable(GL_DEPTH_TEST); //depth test

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
    scene.reset();
    shaderProgram.reset();
    treeModel.reset();
    model.reset();
    if (window != nullptr) {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    glfwTerminate();
}

void Application::run() {
    glm::mat4 view = glm::lookAt(
    glm::vec3(0.0f, 2.5f, 7.0f),  // camera
    glm::vec3(0.0f, 0.5f, 0.0f),  // target
    glm::vec3(0.0f, 1.0f, 0.0f)   // up
    );

    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f),
        800.0f / 600.0f,
        0.1f,
        100.0f
    );

    shaderProgram->use();
    shaderProgram->setMat4("viewMatrix", view);
    shaderProgram->setMat4("projectionMatrix", projection);
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
        scene->draw();

        // display result
        glfwSwapBuffers(window);


        // process events
        glfwPollEvents();
    }
}

