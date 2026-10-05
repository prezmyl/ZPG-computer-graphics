/*
 * Copyright (c) 2026 Martin Nemec
 *
 * File: main.cpp
 * Description: Modern OpenGL - triangle using VBO, VAO and shaders
 */

// Include GLAD
// Only define this in one file
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Standard C++ headers
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <string>
#include <iterator>
#include <iostream>

//modely
#include "Models/sphere.h"


static void error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}


static void key_callback(GLFWwindow* window,
                         int key,
                         int scancode,
                         int action,
                         int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}


static void window_size_callback(GLFWwindow* window,
                                 int width,
                                 int height)
{
    glViewport(0, 0, width, height);
}


// Načtení a kompilace shaderu ze souboru
GLuint createShaderFromFile(GLenum shaderType, const char* shaderFile)
{
    // Vytvoření prázdného shaderu
    GLuint shaderID = glCreateShader(shaderType);

    if (shaderID == 0)
    {
        std::cout << "Unable to create shader" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Načtení obsahu souboru
    std::ifstream file(shaderFile);

    if (!file.is_open())
    {
        std::cout << "Unable to open file "
                  << shaderFile
                  << std::endl;

        glDeleteShader(shaderID);
        exit(EXIT_FAILURE);
    }

    std::string shaderCode(
        (std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>()
    );

    // Zdrojový kód shaderu
    const char* source = shaderCode.c_str();

    glShaderSource(
        shaderID,
        1,
        &source,
        nullptr
    );

    // Kompilace
    glCompileShader(shaderID);

    // Kontrola kompilace
    GLint success;

    glGetShaderiv(
        shaderID,
        GL_COMPILE_STATUS,
        &success
    );

    if (!success)
    {
        char infoLog[1024];

        glGetShaderInfoLog(
            shaderID,
            sizeof(infoLog),
            nullptr,
            infoLog
        );

        std::cout
            << "Shader failed:\n"
            << infoLog
            << std::endl;

        glDeleteShader(shaderID);
        exit(EXIT_FAILURE);
    }

    return shaderID;
}


int main()
{
    // GLFW error callback
    glfwSetErrorCallback(error_callback);

    // Inicializace GLFW
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


    // Vytvoření okna
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


    // Nastavení OpenGL contextu
    glfwMakeContextCurrent(window);

    // VSync
    glfwSwapInterval(1);


    // Inicializace GLAD
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


    // Callbacky
    glfwSetKeyCallback(
        window,
        key_callback
    );

    glfwSetWindowSizeCallback(
        window,
        window_size_callback
    );


    // Velikost framebufferu
    int width;
    int height;

    glfwGetFramebufferSize(
        window,
        &width,
        &height
    );

    glViewport(
        0,
        0,
        width,
        height
    );


    // Informace o OpenGL
    printf(
        "OpenGL Version: %s\n",
        glGetString(GL_VERSION)
    );

    printf(
        "Vendor: %s\n",
        glGetString(GL_VENDOR)
    );

    printf(
        "Renderer: %s\n",
        glGetString(GL_RENDERER)
    );

    printf(
        "GLSL: %s\n",
        glGetString(GL_SHADING_LANGUAGE_VERSION)
    );

    int major;
    int minor;
    int revision;

    glfwGetVersion(
        &major,
        &minor,
        &revision
    );

    printf(
        "Using GLFW %i.%i.%i\n",
        major,
        minor,
        revision
    );


    // =====================================================
    // MODEL - DATA TROJÚHELNÍKU
    // =====================================================

    // Každý vrchol:
    //
    // x, y, z,   r, g, b
    //
    /*float points[] = {

        // horní vrchol - červený
         0.0f,  0.5f, 0.0f,
         1.0f,  0.0f, 0.0f,

        // pravý spodní - zelený
         0.5f, -0.5f, 0.0f,
         0.0f,  1.0f, 0.0f,

        // levý spodní - modrý
        -0.5f, -0.5f, 0.0f,
         0.0f,  0.0f, 1.0f
    };*/

    //2x trojuhelnik
    float points[] = {

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


    // =====================================================
    // VBO - Vertex Buffer Object
    // =====================================================

    GLuint VBO = 0;

    glGenBuffers(
        1,
        &VBO
    );

    glBindBuffer(
        GL_ARRAY_BUFFER,
        VBO
    );

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(sphere),
        sphere,
        GL_STATIC_DRAW
    );


    // =====================================================
    // VAO - Vertex Array Object
    // =====================================================

    GLuint VAO = 0;

    glGenVertexArrays(
        1,
        &VAO
    );

    glBindVertexArray(VAO);


    // atribut 0 = pozice
    glEnableVertexAttribArray(0);

    // atribut 1 = barva
    glEnableVertexAttribArray(1);


    glBindBuffer(
        GL_ARRAY_BUFFER,
        VBO
    );


    // Pozice:
    //
    // x y z
    //
    glVertexAttribPointer(
        0,                  // index atributu
        3,                  // x, y, z
        GL_FLOAT,           // datový typ
        GL_FALSE,           // normalizace
        6 * sizeof(float),  // velikost jednoho vrcholu
        (GLvoid*) 0         // offset
    );


    // Barva:
    //
    // r g b
    //
    glVertexAttribPointer(
        1,                         // index atributu
        3,                         // r, g, b
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),
        (GLvoid*) (3 * sizeof(float))
    );


    // =====================================================
    // SHADERY
    // =====================================================

    GLuint vertexShader =
        createShaderFromFile(
            GL_VERTEX_SHADER,
            "shaders/basic.vert"
        );


    GLuint fragmentShader =
        createShaderFromFile(
            GL_FRAGMENT_SHADER,
            "shaders/basic.frag"
        );


    // =====================================================
    // SHADER PROGRAM
    // =====================================================

    GLuint shaderProgram =
        glCreateProgram();


    glAttachShader(
        shaderProgram,
        vertexShader
    );

    glAttachShader(
        shaderProgram,
        fragmentShader
    );


    glLinkProgram(shaderProgram);


    // Kontrola linkování shader programu
    GLint linkSuccess;

    glGetProgramiv(
        shaderProgram,
        GL_LINK_STATUS,
        &linkSuccess
    );

    if (!linkSuccess)
    {
        char infoLog[1024];

        glGetProgramInfoLog(
            shaderProgram,
            sizeof(infoLog),
            nullptr,
            infoLog
        );

        std::cout
            << "Shader program linking failed:\n"
            << infoLog
            << std::endl;

        glfwDestroyWindow(window);
        glfwTerminate();

        return EXIT_FAILURE;
    }


    // Jednotlivé shadery už po nalinkování
    // shader programu nepotřebujeme.
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);


    // =====================================================
    // HLAVNÍ VYKRESLOVACÍ SMYČKA
    // =====================================================

    while (!glfwWindowShouldClose(window))
    {
        // Barva pozadí
        glClearColor(
            0.15f,
            0.15f,
            0.15f,
            1.0f
        );


        // Vyčištění framebufferu
        glClear(
            GL_COLOR_BUFFER_BIT |
            GL_DEPTH_BUFFER_BIT
        );


        // Použití shader programu
        glUseProgram(shaderProgram);


        // Použití VAO našeho modelu
        glBindVertexArray(VAO);


        // Vykreslení TROJÚHELNÍKU
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


        // Zobrazení výsledku
        glfwSwapBuffers(window);


        // Zpracování událostí
        glfwPollEvents();
    }


    // =====================================================
    // ÚKLID
    // =====================================================

    glDeleteProgram(shaderProgram);

    glDeleteVertexArrays(
        1,
        &VAO
    );

    glDeleteBuffers(
        1,
        &VBO
    );

    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}