#include <chrono>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <thread>
#include <vector>

#include "src/core/Input.hpp"
#include "src/core/Time.hpp"
#include "src/Objects/IUpdate.hpp"
#include "src/Objects/Camera.hpp"
#include "src/Objects/RenderObject.hpp"
#include "src/core/constants.hpp"
#include "src/rendering/model.hpp"

using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;
using FloatDuration = std::chrono::duration<float>;

// Forward Declare
int init(GLFWwindow*& window);
void updateFrameTime(TimePoint& frameStart);
void setup();
void process();
void draw();

// Variables
unsigned int frames = 0;
int frameRate = 120;
float wantedFrameTime = 1.0f / (float)frameRate;
float deltaTime = wantedFrameTime;

std::vector<IUpdate> updateables;
std::vector<RenderObject> renderObjects;

int main()
{
	GLFWwindow* window;
	int result = init(window);
	if (result != 0) { return result; }

	setup();

	Time::deltaTime = wantedFrameTime;

	while (!glfwWindowShouldClose(window))
	{
		TimePoint frameStart = Clock::now();

		// background color set & render
		glClearColor(0.1f, 0.1f, 0.1f, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (Input::keys[GLFW_KEY_ESCAPE])
		{
			glfwSetWindowShouldClose(window, true);
		}

		process();
		draw();

		glfwSwapBuffers(window);

		// event polls
		glfwPollEvents();

		// Update Frame Counter
		updateFrameTime(frameStart);
	}

	return 0;
}

void setup()
{
	Camera::init(glm::normalize(glm::vec3(0.0f, -0.5f, -0.5f)), glm::vec3(100.0f, 125.0f, 100.0f));


}

void process() 
{
	for (int i = 0; i < updateables.size(); i++) 
	{
		updateables[i].Update();
	}
}

void draw()
{
	for (int i = 0; i < renderObjects.size(); i++)
	{
		renderObjects[i].DrawObject();
	}
}

void updateFrameTime(TimePoint& frameStart)
{
	TimePoint frameEnd = Clock::now();
	FloatDuration elapsed = frameEnd - frameStart;
	deltaTime = elapsed.count();  // deltaTime in seconds

	float waitTime = wantedFrameTime - deltaTime;
	if (waitTime > 0.0f) {
		std::this_thread::sleep_for(FloatDuration(waitTime));
	}

	Time::time += deltaTime;
	frames++;
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
	glfwSetCursorPosCallback(window, Input::CursorPosCallback);
	glfwSetKeyCallback(window, Input::KeyCallBack);

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