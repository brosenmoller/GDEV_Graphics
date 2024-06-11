#include <chrono>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <thread>

#include "src/core/Input.hpp"
#include "src/core/Time.hpp"

// Forward Declare
int init(GLFWwindow*& window);
void cursorPosCallback(GLFWwindow* window, double xPos, double yPos);
void keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods);

// Variables
clock_t frameTime = 0;
unsigned int frames = 0;
int frameRate = 120;
float wantedFrameTime = 1.0f / (float)frameRate;
float deltaTime = wantedFrameTime;

float lastMouseX = 0.0f;
float lastMouseY = 0.0f;
bool keys[1024];

const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;

int main()
{
	GLFWwindow* window;
	int result = init(window);
	if (result != 0) { return result; }

	Time::deltaTime = wantedFrameTime;

	while (!glfwWindowShouldClose(window))
	{
		clock_t beginFrame = clock();

		// background color set & render
		glClearColor(0.1f, 0.1f, 0.1f, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



		glfwSwapBuffers(window);

		// event polls
		glfwPollEvents();

		// Update Frame Counter
		clock_t endFrame = clock();
		frameTime = endFrame - beginFrame;
		deltaTime = (frameTime / (float)CLOCKS_PER_SEC);

		float waitTime = wantedFrameTime - deltaTime;
		std::chrono::milliseconds chronoTime = std::chrono::milliseconds((int)((waitTime * 1000.0f)));
		std::this_thread::sleep_for(chronoTime);

		Time::time += wantedFrameTime;
		frames++;
	}


	return 0;
}

int init(GLFWwindow*& window)
{
	glfwInit();

	// Tell GLFW which profile & openGL version we're using
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	// Create Window
	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Rendering Window", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to Create window" << std::endl;
		return -1;
	}

	// Register callbacks
	glfwSetCursorPosCallback(window, cursorPosCallback);
	glfwSetKeyCallback(window, keyCallBack);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set content
	glfwMakeContextCurrent(window);

	// GLad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to load Glad" << std::endl;
		glfwTerminate();
		return -2;
	}

	return 0;
}


void cursorPosCallback(GLFWwindow* window, double xPos, double yPos)
{
	lastMouseX = Input::mousePos.x;
	lastMouseY = Input::mousePos.y;
	Input::mousePos.x = (float)xPos;
	Input::mousePos.y = (float)yPos;

	Input::mouseDelta.x = Input::mousePos.x - lastMouseX;
	Input::mouseDelta.y = Input::mousePos.y - lastMouseY;
}

void keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		Input::keys[key] = true;
	}
	else if (action == GLFW_RELEASE)
	{
		Input::keys[key] = false;
	}
}