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
#include "src/core/Debug.hpp"
#include "src/rendering/SkyBox.hpp"

using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;
using FloatDuration = std::chrono::duration<float>;

// Forward Declare
int init(GLFWwindow*& window);
void updateFrameTime(TimePoint& frameStart);
void setup();
void process();
void draw();
void addRenderObject(Model* model, Material* material, glm::vec3 position = glm::vec3(0, 0, 0));

// Variables
unsigned int frames = 0;
int frameRate = 120;
float wantedFrameTime = 1.0f / (float)frameRate;
float deltaTime = wantedFrameTime;

std::vector<IUpdate*> updateables;
std::vector<RenderObject*> renderObjects;

Model* treeModel;
Material* baseModelMaterial;

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
		glClearColor(0.1f, 0.1f, 0.1f, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (Input::keys[GLFW_KEY_ESCAPE])
		{
			glfwSetWindowShouldClose(window, true);
		}

		process();
		draw();

		glfwSwapBuffers(window);
		glfwPollEvents();
		updateFrameTime(frameStart);
	}

	return 0;
}

void setup()
{
	SkyBox* skyBox = new SkyBox();
	updateables.push_back(skyBox);

	Camera::init(glm::normalize(glm::vec3(0.0f, -0.5f, -0.5f)), glm::vec3(100.0f, 125.0f, 100.0f));
	updateables.push_back(Camera::Instance());
	treeModel = new Model("assets/models/tree/tree.obj");
	baseModelMaterial = new Material("assets/shaders/modelVertex.glsl", "assets/shaders/modelFragment.glsl");

	addRenderObject(treeModel, baseModelMaterial);
	addRenderObject(treeModel, baseModelMaterial, glm::vec3(0, 0, 5));
}

void process() 
{
	for (IUpdate* obj : updateables) 
	{
		if (obj)
		{
			obj->Update();
		}
	}
}

void draw()
{
	for (RenderObject* obj : renderObjects)
	{
		if (obj)
		{
			obj->DrawObject();
		}
	}
}

void addRenderObject(Model* model, Material* material, glm::vec3 position)
{
	RenderObject* treeObject = new RenderObject(model, material, position);
	updateables.push_back(treeObject);
	renderObjects.push_back(treeObject);
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