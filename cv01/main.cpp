/*
 * Copyright (c) 2026 Martin Nemec
 *
 * File: main.cpp
 * Description:  Fixed Function Pipeline.
 */

 //Include GLFW
#include <GLFW/glfw3.h>

//Include GLM
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

//Include the standard C++ headers
#include <stdlib.h>
#include <stdio.h>


float rotationDirection = 1.0f;

static void error_callback(int error, const char* description) { fputs(description, stderr); }

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	printf("key_callback [%d,%d,%d,%d] \n", key, scancode, action, mods);

	if ( key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		rotationDirection *= -1.0f;
	}
}

static void window_focus_callback(GLFWwindow* window, int focused) { printf("window_focus_callback \n"); }

static void window_iconify_callback(GLFWwindow* window, int iconified) { printf("window_iconify_callback \n"); }

static void window_size_callback(GLFWwindow* window, int width, int height) {
	printf("resize %d, %d \n", width, height);
	glViewport(0, 0, width, height);
}

static void cursor_callback(GLFWwindow* window, double x, double y) { printf("cursor_callback \n"); }

static void button_callback(GLFWwindow* window, int button, int action, int mode) {
	if (action == GLFW_PRESS) printf("button_callback [%d,%d,%d]\n", button, action, mode);
}



//GLM test

// Projection matrix: 45 degree FOV, 4:3 aspect ratio, near = 0.01, far = 100
glm::mat4 projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.01f, 100.0f);

// Camera matrix
glm::mat4 view = glm::lookAt(
	glm::vec3(10, 10, 10), // Camera is at (4,3,-3), in World Space
	glm::vec3(0, 0, 0), // and looks at the origin
	glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
);
// Model matrix : an identity matrix (model will be at the origin)
glm::mat4 model = glm::mat4(1.0f);


int main(void)
{
	// Pointer to the GLFW window
	GLFWwindow* window;

	// Initialize GLFW
	if (!glfwInit())
		exit(EXIT_FAILURE);
	window = glfwCreateWindow(640, 480, "ZPG", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	// Set GLFW callback functions
	glfwSetErrorCallback(error_callback);

	glfwSetKeyCallback(window, key_callback);

	glfwSetCursorPosCallback(window, cursor_callback);

	glfwSetMouseButtonCallback(window, button_callback);

	glfwSetWindowFocusCallback(window, window_focus_callback);

	glfwSetWindowIconifyCallback(window, window_iconify_callback);

	glfwSetWindowSizeCallback(window, window_size_callback);

	// Get framebuffer size and set the viewport
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	float ratio = width / (float)height;
	glViewport(0, 0, width, height);

	// Orthographic projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);

	float angle = 0.0f;
	double lastTime = glfwGetTime();
	while (!glfwWindowShouldClose(window))
	{
		// Clear color buffer
		glClear(GL_COLOR_BUFFER_BIT);

		// ModelView matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		double currentTime = glfwGetTime();
		float deltaTime = (float)(currentTime - lastTime);
		lastTime = currentTime;

		angle += rotationDirection * 50.0f * deltaTime;

		glRotatef(angle, 0.f, 0.f, 1.f);

		// Draw a triangle
		glBegin(GL_TRIANGLES);

		// 1. trojúhelník
		glColor3f(1.f, 0.f, 0.f);
		glVertex3f(-0.5f, -0.5f, 0.f);

		glColor3f(0.f, 1.f, 0.f);
		glVertex3f(0.5f, -0.5f, 0.f);

		glColor3f(0.f, 0.f, 1.f);
		glVertex3f(0.5f, 0.5f, 0.f);


		// 2. trojúhelník
		glColor3f(1.f, 0.f, 0.f);
		glVertex3f(-0.5f, -0.5f, 0.f);

		glColor3f(0.f, 0.f, 1.f);
		glVertex3f(0.5f, 0.5f, 0.f);

		glColor3f(1.f, 0.f, 1.f);
		glVertex3f(-0.5f, 0.5f, 0.f);

		glEnd();
		glEnd();

		// Display the rendered frame and process events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	// Clean up and terminate GLFW
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}