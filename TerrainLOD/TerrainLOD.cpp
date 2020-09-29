// CS114FinalProject.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include "Scene.h"

Camera camera(glm::vec3(1024.0f, 1500.0f, 1024.0f));

const GLint WIDTH = 900, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;
bool keys[1024];
GLfloat dT = 0.0f;
GLfloat prevFrame = 0.0f;

void window_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
void key_callback(GLFWwindow *window, int key, int scandcode, int action, int mode)
{
	if (key >= 0 && key < 1024)
	{
		if (GLFW_PRESS == action)
		{
			keys[key] = true;
		}
		else if (GLFW_RELEASE == action)
		{
			keys[key] = false;
		}
	}
}
void do_movement(bool(&keys)[1024], GLfloat delta)
{
	if (keys[GLFW_KEY_DOWN])
	{
		camera.setPosition(glm::vec3(0.0f, 3.0f, 0.0f));
	}
	if (keys[GLFW_KEY_UP])
	{
		camera.setPosition(glm::vec3(0.0f, -3.0f, 0.0f));
	}
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Terrain LOD", nullptr, nullptr);
	if (nullptr == window)
	{
		std::cout << "ERROR::GLFW WINDOW FAILED TO CREATE" << std::endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, window_size_callback);
	glfwSetKeyCallback(window, key_callback);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "ERROR:GLAD CONTEXT FAILED" << std::endl;
		return -1;
	}
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	Scene sceneObj;
	while (!glfwWindowShouldClose(window))
	{
		GLfloat frame = glfwGetTime();
		dT = frame - prevFrame;
		prevFrame = frame;
		glfwPollEvents();
		glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);
		if (SCREEN_HEIGHT > 0)
		{
			bool draw_wireframe = false;
			do_movement(keys, dT);
			if (keys[GLFW_KEY_T])
			{
				if (draw_wireframe)
					draw_wireframe = false;
				else
					draw_wireframe = true;
			}
			glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glm::mat4 view = camera.getViewMatrix();
			glm::mat4 projection = camera.getProjectionMatrix(SCREEN_WIDTH, SCREEN_HEIGHT);
			sceneObj.drawScene(camera, view, projection, draw_wireframe);
			glfwSwapBuffers(window);
		}
	}
	sceneObj.erase();
	glfwTerminate();
	return EXIT_SUCCESS;
}
