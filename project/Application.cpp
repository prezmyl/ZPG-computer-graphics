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
#include "RotationAnimation.h"
#include <glm/gtc/matrix_transform.hpp>

#include "Models/bushes.h"
#include "Models/tree.h"

#include "Models/sphere.h"

#include <random>

//static - original callbacks
static void error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

static void key_callback(GLFWwindow* window, int key,
                         int scancode, int action, int mods)
{
    auto* app = static_cast<Application*>(
        glfwGetWindowUserPointer(window) //get the address of app instance, since this is static metod
    );

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    if (app && key == GLFW_KEY_1 && action == GLFW_PRESS) {
        app->setActiveScene(0);
    }

    if (app && key == GLFW_KEY_2 && action == GLFW_PRESS) {
        app->setActiveScene(1);
    }

    if (app && key == GLFW_KEY_3 && action == GLFW_PRESS) {
        app->setActiveScene(2);
    }
}

static void window_size_callback(GLFWwindow* window,
                                 int width, int height)
{
    glViewport(0, 0, width, height);
}



void Application::createResources() {

    model = std::make_unique<Model>(bushes, static_cast<GLsizei>(sizeof(bushes) / (6 * sizeof(float))));
    treeModel = std::make_unique<Model>(tree, static_cast<GLsizei>(sizeof(tree) / (6 * sizeof(float))));
    sphereModel = std::make_unique<Model>(sphere, static_cast<GLsizei>(sizeof(sphere) / (6 * sizeof(float))));

    Shader vertexShader(GL_VERTEX_SHADER,"shaders/normal.vert");
    Shader fragmentShader(GL_FRAGMENT_SHADER, "shaders/normal.frag");

    shaderProgram = std::make_unique<ShaderProgram>(vertexShader, fragmentShader);

    auto sceneForest = std::make_unique<Scene>();
    //addScene(std::move(sceneForest)); uz bych predal vlastnictvi scenes[0] a to bych musel pouzivat dale -> tohle presunu az nokonec a tam lokalni forestScene predam az do scenes[0]
    //==== GENERETED AI =====
    // lambda func temporary for transforamtion
    auto addInstance = [&](Model* mesh, glm::vec3 position, float size, bool animate = false) {
        auto transform = std::make_unique<CompositeTransformation>();

        transform->addTransformation(
            std::make_unique<Scale>(glm::vec3(size))
        );
        if (animate) {

            auto rotation = std::make_unique<Rotation>(glm::vec3(0.0f, 1.0f, 0.0f), 0.0f);
            auto animation = std::make_unique<RotationAnimation>(rotation.get(), 2.0f); //same rotation
            sceneForest->addAnimation(std::move(animation)); //this is the use of the same rotation for animations colection
            transform->addTransformation(std::move(rotation)); //same rotation

        }

        transform->addTransformation(
            std::make_unique<Translation>(position)
        );

        sceneForest->addObject(std::make_unique<DrawableObject>(
            mesh,
            shaderProgram.get(),
            std::move(transform)
        ));
    };

    std::mt19937 rng(42);  // pevný seed pro opakovatelnou scénu

    std::uniform_real_distribution<float> pos(-3.5f, 3.5f);
    std::uniform_real_distribution<float> treeSize(0.20f, 0.35f);
    std::uniform_real_distribution<float> bushSize(1.0f, 2.0f);

    // Trees
    for (int i = 0; i < 20; ++i) {
        addInstance(
            treeModel.get(),
            glm::vec3(pos(rng), 0.0f, pos(rng)),
            treeSize(rng),
            i == 0
        );
    }

    // Bushes
    for (int i = 0; i < 30; ++i) {
        addInstance(
            model.get(),
            glm::vec3(pos(rng), 0.0f, pos(rng)),
            bushSize(rng)
        );
    }

    addScene(std::move(sceneForest)); //az tady ji predam


    auto sceneLogin = std::make_unique<Scene>();
    addScene(std::move(sceneLogin));

// ======= SOLOAR SYSTEM============
    auto sceneSolarSystem = std::make_unique<Scene>();

    // ===== SUN =====
    auto sunTransform = std::make_unique<CompositeTransformation>();

    sunTransform->addTransformation(
        std::make_unique<Scale>(glm::vec3(0.65f))
    );

    sceneSolarSystem->addObject(std::make_unique<DrawableObject>(
        sphereModel.get(),
        shaderProgram.get(),
        std::move(sunTransform)
    ));

    // ===== EARTH =====
    auto earthTransform = std::make_unique<CompositeTransformation>();

    // 1. Size of Earth
    earthTransform->addTransformation(
        std::make_unique<Scale>(glm::vec3(0.25f))
    );

    // 2. Rotation around its own axis
    auto earthSpin = std::make_unique<Rotation>(
        glm::vec3(0.0f, 1.0f, 0.0f), 0.0f
    );

    sceneSolarSystem->addAnimation(
        std::make_unique<RotationAnimation>(earthSpin.get(), 2.0f)
    );

    earthTransform->addTransformation(std::move(earthSpin));

    // 3. Distance from Sun
    earthTransform->addTransformation(
        std::make_unique<Translation>(glm::vec3(2.5f, 0.0f, 0.0f))
    );

    // 4. Revolution around Sun
    auto earthOrbit = std::make_unique<Rotation>(
        glm::vec3(0.0f, 1.0f, 0.0f), 0.0f
    );

    sceneSolarSystem->addAnimation(
        std::make_unique<RotationAnimation>(earthOrbit.get(), 0.5f)
    );

    earthTransform->addTransformation(std::move(earthOrbit));

    // Register Earth
    sceneSolarSystem->addObject(std::make_unique<DrawableObject>(
        sphereModel.get(),
        shaderProgram.get(),
        std::move(earthTransform)
    ));


    // ===== MOON =====
    auto moonTransform = std::make_unique<CompositeTransformation>();

    // 1. Moon size
    moonTransform->addTransformation(
        std::make_unique<Scale>(glm::vec3(0.10f))
    );

    // 2. Distance from Earth
    moonTransform->addTransformation(
        std::make_unique<Translation>(
            glm::vec3(0.55f, 0.0f, 0.0f)
        )
    );

    // 3. Moon revolves around Earth
    auto moonOrbit = std::make_unique<Rotation>(
        glm::vec3(0.0f, 1.0f, 0.0f), 0.0f
    );

    sceneSolarSystem->addAnimation(
        std::make_unique<RotationAnimation>(
            moonOrbit.get(), 2.0f
        )
    );

    moonTransform->addTransformation(std::move(moonOrbit));

    // 4. Earth distance from Sun
    moonTransform->addTransformation(
        std::make_unique<Translation>(
            glm::vec3(2.5f, 0.0f, 0.0f)
        )
    );

    // 5. Moon follows Earth's orbit around Sun
    auto earthOrbitForMoon = std::make_unique<Rotation>(
        glm::vec3(0.0f, 1.0f, 0.0f), 0.0f
    );

    sceneSolarSystem->addAnimation(
        std::make_unique<RotationAnimation>(
            earthOrbitForMoon.get(), 0.5f
        )
    );

    moonTransform->addTransformation(std::move(earthOrbitForMoon));

    // Register Moon
    sceneSolarSystem->addObject(
        std::make_unique<DrawableObject>(
            sphereModel.get(),
            shaderProgram.get(),
            std::move(moonTransform)
        )
    );


    // Transfer ownership of completed scene
    addScene(std::move(sceneSolarSystem));


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

    //set up user pointer for switichnig scenes (intant of application is regitred)
    glfwSetWindowUserPointer(window, this);

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
    scenes.clear(); //clear all scenes followed by destruction of all means
    shaderProgram.reset();
    treeModel.reset();
    model.reset();
    sphereModel.reset();
    if (window != nullptr) {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    glfwTerminate();
}

void Application::run() {
    glm::mat4 view = glm::lookAt(
    glm::vec3(5.0f, 1.0f, 12.0f),
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f)
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
        Scene* activeScene = scenes[activeSceneIdx].get();

        activeScene->update(glfwGetTime()); //added
        activeScene->draw();

        // display result
        glfwSwapBuffers(window);


        // process events
        glfwPollEvents();
    }
}

void Application::addScene(std::unique_ptr<Scene> scene) {
    scenes.push_back(std::move(scene));
}

void Application::setActiveScene(std::size_t idx) {
    if (idx < scenes.size()) {
        activeSceneIdx = idx;
    }
}

