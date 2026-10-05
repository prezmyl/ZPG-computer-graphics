/*
 * Author: YOUR NAME
 * Login: YOUR LOGIN
 *
 * ZPG - Exercise 3
 * Basic transformations using shader uniforms.
 *
 * AI assistance was used when preparing this source code.
 */

#include <cstdio>
#include <cstdlib>
#include <string>

// GLAD - implementation must be defined only once
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>

// GLFW
#include <GLFW/glfw3.h>

// Model
#include "Models/sphere.h"


static void errorCallback(int error, const char* description)
{
    std::fprintf(
        stderr,
        "GLFW error %d: %s\n",
        error,
        description
    );
}


static GLuint compileShader(GLenum shaderType, const char* source)
{
    GLuint shader = glCreateShader(shaderType);

    glShaderSource(
        shader,
        1,
        &source,
        nullptr
    );

    glCompileShader(shader);

    GLint success = GL_FALSE;

    glGetShaderiv(
        shader,
        GL_COMPILE_STATUS,
        &success
    );

    if (!success)
    {
        GLint length = 0;

        glGetShaderiv(
            shader,
            GL_INFO_LOG_LENGTH,
            &length
        );

        std::string log(length, '\0');

        glGetShaderInfoLog(
            shader,
            length,
            nullptr,
            log.data()
        );

        std::fprintf(
            stderr,
            "Shader compilation error:\n%s\n",
            log.c_str()
        );

        glDeleteShader(shader);
        std::exit(EXIT_FAILURE);
    }

    return shader;
}


static GLuint createShaderProgram()
{
    const char* vertexShaderSource = R"GLSL(
#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;

uniform vec3 uTranslation;
uniform float uScale;
uniform float uAngle;

out vec3 vNormal;

void main()
{
    vec3 p = aPosition;

    // Scale
    p.x = p.x * uScale;
    p.y = p.y * uScale;
    p.z = p.z * uScale;

    // Rotation around Y axis
    float x = cos(uAngle) * p.x + sin(uAngle) * p.z;
    float y = p.y;
    float z = -sin(uAngle) * p.x + cos(uAngle) * p.z;

    p.x = x;
    p.y = y;
    p.z = z;

    // Translation
    p.x = p.x + uTranslation.x;
    p.y = p.y + uTranslation.y;
    p.z = p.z + uTranslation.z;

    gl_Position = vec4(p, 1.0);

    vNormal = aNormal;
}
)GLSL";


    const char* fragmentShaderSource = R"GLSL(
#version 330 core

in vec3 vNormal;

out vec4 fragColor;

void main()
{
    vec3 n = normalize(vNormal);

    // Display normal as color using absolute value
    fragColor = vec4(abs(n), 1.0);
}
)GLSL";


    GLuint vertexShader =
        compileShader(
            GL_VERTEX_SHADER,
            vertexShaderSource
        );

    GLuint fragmentShader =
        compileShader(
            GL_FRAGMENT_SHADER,
            fragmentShaderSource
        );


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


    GLint success = GL_FALSE;

    glGetProgramiv(
        shaderProgram,
        GL_LINK_STATUS,
        &success
    );

    if (!success)
    {
        GLint length = 0;

        glGetProgramiv(
            shaderProgram,
            GL_INFO_LOG_LENGTH,
            &length
        );

        std::string log(length, '\0');

        glGetProgramInfoLog(
            shaderProgram,
            length,
            nullptr,
            log.data()
        );

        std::fprintf(
            stderr,
            "Shader program linking error:\n%s\n",
            log.c_str()
        );

        glDeleteProgram(shaderProgram);
        std::exit(EXIT_FAILURE);
    }


    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}


int main()
{
    glfwSetErrorCallback(errorCallback);


    // -----------------------------------------------------
    // GLFW initialization
    // -----------------------------------------------------

    if (!glfwInit())
    {
        std::fprintf(
            stderr,
            "Unable to initialize GLFW.\n"
        );

        return EXIT_FAILURE;
    }


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

    glfwWindowHint(
        GLFW_OPENGL_FORWARD_COMPAT,
        GL_TRUE
    );


    GLFWwindow* window =
        glfwCreateWindow(
            800,
            600,
            "ZPG - Transformations",
            nullptr,
            nullptr
        );


    if (!window)
    {
        glfwTerminate();

        return EXIT_FAILURE;
    }


    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);


    // -----------------------------------------------------
    // GLAD initialization
    // -----------------------------------------------------

    if (!gladLoadGL(
            (GLADloadfunc) glfwGetProcAddress
        ))
    {
        std::fprintf(
            stderr,
            "Unable to initialize GLAD.\n"
        );

        glfwDestroyWindow(window);
        glfwTerminate();

        return EXIT_FAILURE;
    }


    std::printf(
        "OpenGL: %s\n",
        glGetString(GL_VERSION)
    );

    std::printf(
        "GLSL: %s\n",
        glGetString(GL_SHADING_LANGUAGE_VERSION)
    );


    // -----------------------------------------------------
    // Viewport and depth test
    // -----------------------------------------------------

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


    glEnable(GL_DEPTH_TEST);


    // -----------------------------------------------------
    // VBO
    // -----------------------------------------------------

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


    // -----------------------------------------------------
    // VAO
    // -----------------------------------------------------

    GLuint VAO = 0;

    glGenVertexArrays(
        1,
        &VAO
    );

    glBindVertexArray(VAO);


    glBindBuffer(
        GL_ARRAY_BUFFER,
        VBO
    );


    // Position: x, y, z
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),
        (void*) 0
    );


    // Normal: nx, ny, nz
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),
        (void*) (3 * sizeof(float))
    );


    // -----------------------------------------------------
    // Shader program
    // -----------------------------------------------------

    GLuint shaderProgram =
        createShaderProgram();


    glUseProgram(shaderProgram);


    GLint translationLocation =
        glGetUniformLocation(
            shaderProgram,
            "uTranslation"
        );

    GLint scaleLocation =
        glGetUniformLocation(
            shaderProgram,
            "uScale"
        );

    GLint angleLocation =
        glGetUniformLocation(
            shaderProgram,
            "uAngle"
        );


    if (translationLocation == -1)
    {
        std::fprintf(
            stderr,
            "Uniform uTranslation was not found.\n"
        );
    }

    if (scaleLocation == -1)
    {
        std::fprintf(
            stderr,
            "Uniform uScale was not found.\n"
        );
    }

    if (angleLocation == -1)
    {
        std::fprintf(
            stderr,
            "Uniform uAngle was not found.\n"
        );
    }


    // -----------------------------------------------------
    // Transformation values
    // -----------------------------------------------------

    float translationX = 0.0f;
    float translationY = 0.0f;
    float translationZ = 0.0f;

    float scale = 0.5f;

    float angle = 0.0f;


    double previousTime =
        glfwGetTime();


    // -----------------------------------------------------
    // Rendering loop
    // -----------------------------------------------------

    while (!glfwWindowShouldClose(window))
    {
        double currentTime =
            glfwGetTime();

        float deltaTime =
            static_cast<float>(
                currentTime - previousTime
            );

        previousTime =
            currentTime;


        glfwPollEvents();


        // -------------------------------------------------
        // Keyboard control
        // -------------------------------------------------

        if (glfwGetKey(
                window,
                GLFW_KEY_ESCAPE
            ) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(
                window,
                GLFW_TRUE
            );
        }


        // Translation
        if (glfwGetKey(
                window,
                GLFW_KEY_LEFT
            ) == GLFW_PRESS)
        {
            translationX -=
                0.5f * deltaTime;
        }


        if (glfwGetKey(
                window,
                GLFW_KEY_RIGHT
            ) == GLFW_PRESS)
        {
            translationX +=
                0.5f * deltaTime;
        }


        if (glfwGetKey(
                window,
                GLFW_KEY_UP
            ) == GLFW_PRESS)
        {
            translationY +=
                0.5f * deltaTime;
        }


        if (glfwGetKey(
                window,
                GLFW_KEY_DOWN
            ) == GLFW_PRESS)
        {
            translationY -=
                0.5f * deltaTime;
        }


        // Scale
        if (glfwGetKey(
                window,
                GLFW_KEY_EQUAL
            ) == GLFW_PRESS)
        {
            scale +=
                0.5f * deltaTime;
        }


        if (glfwGetKey(
                window,
                GLFW_KEY_MINUS
            ) == GLFW_PRESS)
        {
            scale -=
                0.5f * deltaTime;
        }


        if (scale < 0.05f)
        {
            scale = 0.05f;
        }


        // Rotation around Y axis
        if (glfwGetKey(
                window,
                GLFW_KEY_A
            ) == GLFW_PRESS)
        {
            angle +=
                1.5f * deltaTime;
        }


        if (glfwGetKey(
                window,
                GLFW_KEY_D
            ) == GLFW_PRESS)
        {
            angle -=
                1.5f * deltaTime;
        }


        // -------------------------------------------------
        // Send transformation values to shader
        // -------------------------------------------------

        glUseProgram(shaderProgram);


        if (translationLocation != -1)
        {
            glUniform3f(
                translationLocation,
                translationX,
                translationY,
                translationZ
            );
        }


        if (scaleLocation != -1)
        {
            glUniform1f(
                scaleLocation,
                scale
            );
        }


        if (angleLocation != -1)
        {
            glUniform1f(
                angleLocation,
                angle
            );
        }


        // -------------------------------------------------
        // Rendering
        // -------------------------------------------------

        glClearColor(
            0.1f,
            0.1f,
            0.1f,
            1.0f
        );

        glClear(
            GL_COLOR_BUFFER_BIT |
            GL_DEPTH_BUFFER_BIT
        );


        glBindVertexArray(VAO);


        glDrawArrays(
            GL_TRIANGLES,
            0,
            2880
        );


        glfwSwapBuffers(window);
    }


    // -----------------------------------------------------
    // Cleanup
    // -----------------------------------------------------

    glDeleteProgram(
        shaderProgram
    );

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