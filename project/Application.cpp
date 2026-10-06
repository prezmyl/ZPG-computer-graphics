//
// Created by xpolas on 10/5/26.
//

#include "Application.h"


bool Application::initialize() {
    // GLFW error callback
    glfwSetErrorCallback(error_callback);

    // Inicialization GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "GLFW initialization failed\n");
        return EXIT_FAILURE;
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
    GLFWwindow* window = glfwCreateWindow(
        800,
        600,
        "ZPG - cv02 - triangle",
        nullptr,
        nullptr
    );

    if (!window)
    {
        glfwTerminate();
        return EXIT_FAILURE;
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

        glfwDestroyWindow(window);
        glfwTerminate();

        return EXIT_FAILURE;
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


        // use shader program
        glUseProgram(shaderProgram);


        // use VAO of the model
        glBindVertexArray(VAO);


        // Draw
        //
        // GL_TRIANGLES = kreslíme trojúhelníky
        // 0            = začínáme prvním vrcholem
        // 3            = máme tři vrcholy
        //
        glDrawArrays(
            GL_TRIANGLES,
            0,
            2880
        );


        // display result
        glfwSwapBuffers(window);


        // process events
        glfwPollEvents();
    }
}

