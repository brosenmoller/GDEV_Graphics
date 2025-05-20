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
void createCubeMesh(GLuint& VAO, int& numVertices, int& numIndices);
void renderSkyBox();

// Variables
unsigned int frames = 0;
int frameRate = 120;
float wantedFrameTime = 1.0f / (float)frameRate;
float deltaTime = wantedFrameTime;

std::vector<IUpdate*> updateables;
std::vector<RenderObject*> renderObjects;

GLuint skyProgramID;

GLuint boxVAO;
int boxNumVertices;
int boxNumIndices;

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

		// background color set & render
		glClearColor(0.1f, 0.1f, 0.1f, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (Input::keys[GLFW_KEY_ESCAPE])
		{
			glfwSetWindowShouldClose(window, true);
		}

		process();
		renderSkyBox();
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
	Material::createProgram(skyProgramID, "assets/shaders/skyVertex.glsl", "assets/shaders/skyFragment.glsl");
	createCubeMesh(boxVAO, boxNumVertices, boxNumIndices);

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

void renderSkyBox()
{
	glm::mat4 transform = glm::mat4(1.0f);
	transform = glm::translate(transform, Camera::Instance()->position);
	transform = glm::scale(transform, glm::vec3(10.0f, 10.0f, 10.0f));

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH);
	glDisable(GL_DEPTH_TEST);

	glUseProgram(skyProgramID);

	glUniformMatrix4fv(glGetUniformLocation(skyProgramID, "transform"), 1, GL_FALSE, glm::value_ptr(transform));
	glUniformMatrix4fv(glGetUniformLocation(skyProgramID, "view"), 1, GL_FALSE, glm::value_ptr(Camera::Instance()->view));
	glUniformMatrix4fv(glGetUniformLocation(skyProgramID, "projection"), 1, GL_FALSE, glm::value_ptr(Camera::Instance()->projection));

	glUniform3fv(glGetUniformLocation(skyProgramID, "cameraPosition"), 1, glm::value_ptr(Camera::Instance()->position));
	glUniform3fv(glGetUniformLocation(skyProgramID, "lightDirection"), 1, glm::value_ptr(Camera::Instance()->lightDirection));

	glBindVertexArray(boxVAO);
	glDrawElements(GL_TRIANGLES, boxNumIndices, GL_UNSIGNED_INT, 0);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH);
	glEnable(GL_DEPTH_TEST);
}

void createCubeMesh(GLuint& VAO, int& numVertices, int& numIndices)  //VAO = Vertex Array Object
{
	// need 24 vertices for normal/uv-mapped Cube
	float vertices[] = {
		// positions            //colors            // tex coords   // normals          //tangents      //bitangents
		0.5f, -0.5f, -0.5f,     1.0f, 1.0f, 1.0f,   1.f, 1.f,       0.f, -1.f, 0.f,     -1.f, 0.f, 0.f,  0.f, 0.f, 1.f,
		0.5f, -0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   1.f, 0.f,       0.f, -1.f, 0.f,     -1.f, 0.f, 0.f,  0.f, 0.f, 1.f,
		-0.5f, -0.5f, 0.5f,     1.0f, 1.0f, 1.0f,   0.f, 0.f,       0.f, -1.f, 0.f,     -1.f, 0.f, 0.f,  0.f, 0.f, 1.f,
		-0.5f, -0.5f, -.5f,     1.0f, 1.0f, 1.0f,   0.f, 1.f,       0.f, -1.f, 0.f,     -1.f, 0.f, 0.f,  0.f, 0.f, 1.f,

		0.5f, 0.5f, -0.5f,      1.0f, 1.0f, 1.0f,   1.f, 1.f,       1.f, 0.f, 0.f,     0.f, -1.f, 0.f,  0.f, 0.f, 1.f,
		0.5f, 0.5f, 0.5f,       1.0f, 1.0f, 1.0f,   1.f, 0.f,       1.f, 0.f, 0.f,     0.f, -1.f, 0.f,  0.f, 0.f, 1.f,

		0.5f, 0.5f, 0.5f,       1.0f, 1.0f, 1.0f,   1.f, 0.f,       0.f, 0.f, 1.f,     1.f, 0.f, 0.f,  0.f, -1.f, 0.f,
		-0.5f, 0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   0.f, 0.f,       0.f, 0.f, 1.f,     1.f, 0.f, 0.f,  0.f, -1.f, 0.f,

		-0.5f, 0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   0.f, 0.f,      -1.f, 0.f, 0.f,     0.f, 1.f, 0.f,  0.f, 0.f, 1.f,
		-0.5f, 0.5f, -.5f,      1.0f, 1.0f, 1.0f,   0.f, 1.f,      -1.f, 0.f, 0.f,     0.f, 1.f, 0.f,  0.f, 0.f, 1.f,

		-0.5f, 0.5f, -.5f,      1.0f, 1.0f, 1.0f,   0.f, 1.f,      0.f, 0.f, -1.f,     1.f, 0.f, 0.f,  0.f, 1.f, 0.f,
		0.5f, 0.5f, -0.5f,      1.0f, 1.0f, 1.0f,   1.f, 1.f,      0.f, 0.f, -1.f,     1.f, 0.f, 0.f,  0.f, 1.f, 0.f,

		-0.5f, 0.5f, -.5f,      1.0f, 1.0f, 1.0f,   1.f, 1.f,       0.f, 1.f, 0.f,     1.f, 0.f, 0.f,  0.f, 0.f, 1.f,
		-0.5f, 0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   1.f, 0.f,       0.f, 1.f, 0.f,     1.f, 0.f, 0.f,  0.f, 0.f, 1.f,

		0.5f, -0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   1.f, 1.f,       0.f, 0.f, 1.f,     1.f, 0.f, 0.f,  0.f, -1.f, 0.f,
		-0.5f, -0.5f, 0.5f,     1.0f, 1.0f, 1.0f,   0.f, 1.f,       0.f, 0.f, 1.f,     1.f, 0.f, 0.f,  0.f, -1.f, 0.f,

		-0.5f, -0.5f, 0.5f,     1.0f, 1.0f, 1.0f,   1.f, 0.f,       -1.f, 0.f, 0.f,     0.f, 1.f, 0.f,  0.f, 0.f, 1.f,
		-0.5f, -0.5f, -.5f,     1.0f, 1.0f, 1.0f,   1.f, 1.f,       -1.f, 0.f, 0.f,     0.f, 1.f, 0.f,  0.f, 0.f, 1.f,

		-0.5f, -0.5f, -.5f,     1.0f, 1.0f, 1.0f,   0.f, 0.f,       0.f, 0.f, -1.f,     1.f, 0.f, 0.f,  0.f, 1.f, 0.f,
		0.5f, -0.5f, -0.5f,     1.0f, 1.0f, 1.0f,   1.f, 0.f,       0.f, 0.f, -1.f,     1.f, 0.f, 0.f,  0.f, 1.f, 0.f,

		0.5f, -0.5f, -0.5f,     1.0f, 1.0f, 1.0f,   0.f, 1.f,       1.f, 0.f, 0.f,     0.f, -1.f, 0.f,  0.f, 0.f, 1.f,
		0.5f, -0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   0.f, 0.f,       1.f, 0.f, 0.f,     0.f, -1.f, 0.f,  0.f, 0.f, 1.f,

		0.5f, 0.5f, -0.5f,      1.0f, 1.0f, 1.0f,   0.f, 1.f,       0.f, 1.f, 0.f,     1.f, 0.f, 0.f,  0.f, 0.f, 1.f,
		0.5f, 0.5f, 0.5f,       1.0f, 1.0f, 1.0f,   0.f, 0.f,       0.f, 1.f, 0.f,     1.f, 0.f, 0.f,  0.f, 0.f, 1.f
	};

	unsigned int indices[] = {  // note that we start from 0!
		// DOWN
		0, 1, 2,   // first triangle
		0, 2, 3,    // second triangle
		// BACK
		14, 6, 7,   // first triangle
		14, 7, 15,    // second triangle
		// RIGHT
		20, 4, 5,   // first triangle
		20, 5, 21,    // second triangle
		// LEFT
		16, 8, 9,   // first triangle
		16, 9, 17,    // second triangle
		// FRONT
		18, 10, 11,   // first triangle
		18, 11, 19,    // second triangle
		// UP
		22, 12, 13,   // first triangle
		22, 13, 23,    // second triangle
	};

	int stride = (3 + 3 + 2 + 3 + 3 + 3) * sizeof(float);
	numVertices = sizeof(vertices) / stride;
	numIndices = sizeof(indices) / sizeof(int);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	GLuint EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(3);

	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, stride, (void*)(11 * sizeof(float)));
	glEnableVertexAttribArray(4);

	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, stride, (void*)(14 * sizeof(float)));
	glEnableVertexAttribArray(5);
}