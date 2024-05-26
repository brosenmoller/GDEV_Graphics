#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#define GLM_ENABLE_EXPERIMENTAL
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

int init(GLFWwindow* &window);
void processInput(GLFWwindow*& window);
void createCubeMesh(GLuint& VAO, int& numVertices, int& numIndices);
void createShaders();
void createProgram(GLuint& programID, const char* vertexShaderPath, const char* fragmentShaderPath);
void loadFile(const char* filename, char*& output);
void renderSkyBox();
void renderCube();
void renderTerrain();
GLuint GeneratePlane(const char* heightmap, GLenum format, int comp, float hScale, float xzScale, unsigned int& indexCount, unsigned int& heightmapID);

// Window Callbacks
void cursorPosCallback(GLFWwindow* window, double xPos, double yPos);
void keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods);

bool keys[1024];

GLuint loadTexture(const char* path);

// Program IDs
GLuint simpleProgramID, skyProgramID, terrainProgramID;

const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;

clock_t frameTime = 0;
unsigned int frames = 0;
double frameRate = 120;
double wantedFrameTime = 1 / frameRate;
double deltaTime = wantedFrameTime;
double programTime = 0;

// World Data
glm::mat4 view;
glm::mat4 projection = glm::perspective(glm::radians(50.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 5000.0f);
glm::vec3 pointLightPosition = glm::vec3(1.0f, 2.5f, -1.0f);
glm::vec3 lightDirection = glm::normalize(glm::vec3(0.0f, -0.5f, -0.5f));

// Cube Data
GLuint boxTexture;
GLuint boxNormalTex;

GLuint boxVAO;
int boxNumVertices;
int boxNumIndices;

// Terrain Data
GLuint terrainVAO, terrainIndexCount, heightMapID;

// Camera Data
glm::vec3 cameraPosition = glm::vec3(100.0f, 125.0f, 100.0f);
glm::vec3 cameraForward = glm::vec3(0, 0, 1);
glm::vec3 cameraUp = glm::vec3(0, 1, 0);
glm::quat camQuat = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));

float camLastX, camLastY;
bool camIsFirstMove = true;
float camYaw, camPitch;

int main()
{
	GLFWwindow* window;
	int result = init(window);
	if (result != 0) { return result; }

	createShaders();

	createCubeMesh(boxVAO, boxNumVertices, boxNumIndices);
	terrainVAO = GeneratePlane("assets/textures/Heightmap2.png", GL_RGBA, 4, 100.0f, 5.0f, terrainIndexCount, heightMapID);

	// Set texture channels
	boxTexture = loadTexture("assets/textures/container2.png");
	boxNormalTex = loadTexture("assets/textures/container2_normalmap.png");

	glUseProgram(simpleProgramID);
	glUniform1i(glGetUniformLocation(simpleProgramID, "mainTex"), 0);
	glUniform1i(glGetUniformLocation(simpleProgramID, "normalTex"), 1);

	// Create Viewport
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	// Game render loop
	while (!glfwWindowShouldClose(window))
	{
		clock_t beginFrame = clock();

		// input handling
		processInput(window);

		// background color set & render
		glClearColor(0.1f, 0.1f, 0.1f, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		view = glm::lookAt(cameraPosition, cameraPosition + cameraForward, cameraUp);

		renderSkyBox();
		renderTerrain();
		//renderCube();

		glfwSwapBuffers(window);

		// event polls
		glfwPollEvents();

		// Update Frame Counter
		clock_t endFrame = clock();

		frameTime = endFrame - beginFrame;
		deltaTime = (frameTime / (double)CLOCKS_PER_SEC);

		int milliesecondWaitTime = (int)((wantedFrameTime - deltaTime) * 1000.0f);
		std::this_thread::sleep_for(std::chrono::milliseconds(milliesecondWaitTime));

		programTime += wantedFrameTime;
		frames++;
	}

	// terminate
	return 0;
}

void renderSkyBox()
{
	glm::mat4 transform = glm::mat4(1.0f);
	transform = glm::translate(transform, cameraPosition);
	transform = glm::scale(transform, glm::vec3(10.0f, 10.0f, 10.0f));
	
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH);
	glDisable(GL_DEPTH_TEST);

	glUseProgram(skyProgramID);

	glUniformMatrix4fv(glGetUniformLocation(skyProgramID, "transform"),  1, GL_FALSE, glm::value_ptr(transform));
	glUniformMatrix4fv(glGetUniformLocation(skyProgramID, "view"),       1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(skyProgramID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	glUniform3fv(glGetUniformLocation(skyProgramID, "cameraPosition"),   1, glm::value_ptr(cameraPosition));
	glUniform3fv(glGetUniformLocation(skyProgramID, "lightDirection"),   1, glm::value_ptr(lightDirection));

	glBindVertexArray(boxVAO);
	glDrawElements(GL_TRIANGLES, boxNumIndices, GL_UNSIGNED_INT, 0);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH);
	glEnable(GL_DEPTH_TEST);
}

void renderTerrain()
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH);
	glEnable(GL_DEPTH_TEST);

	glUseProgram(terrainProgramID);

	glm::mat4 transform = glm::mat4(1.0f);

	glUniformMatrix4fv(glGetUniformLocation(terrainProgramID, "transform"), 1, GL_FALSE, glm::value_ptr(transform));
	glUniformMatrix4fv(glGetUniformLocation(terrainProgramID, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(terrainProgramID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	glUniform3fv(glGetUniformLocation(terrainProgramID, "cameraPosition"), 1, glm::value_ptr(cameraPosition));
	glUniform3fv(glGetUniformLocation(terrainProgramID, "lightDirection"), 1, glm::value_ptr(lightDirection));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, heightMapID);

	glBindVertexArray(terrainVAO);
	glDrawElements(GL_TRIANGLES, terrainIndexCount, GL_UNSIGNED_INT, 0);
}

void renderCube()
{
	glm::mat4 transform = glm::mat4(1.0f);
	transform = glm::rotate(transform, glm::radians(45.0f), glm::vec3(0, 1, 0));
	transform = glm::scale(transform, glm::vec3(1, 1, 1));
	transform = glm::translate(transform, glm::vec3(0, 0, 0));

	glUseProgram(simpleProgramID);

	glUniformMatrix4fv(glGetUniformLocation(simpleProgramID, "transform"), 1, GL_FALSE, glm::value_ptr(transform));
	glUniformMatrix4fv(glGetUniformLocation(simpleProgramID, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(simpleProgramID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	glUniform3fv(glGetUniformLocation(simpleProgramID, "lightPosition"), 1, glm::value_ptr(pointLightPosition));
	glUniform3fv(glGetUniformLocation(simpleProgramID, "cameraPosition"), 1, glm::value_ptr(cameraPosition));

	glUniform1f(glGetUniformLocation(simpleProgramID, "time"), (float)programTime);

	glUniform3fv(glGetUniformLocation(simpleProgramID, "ambientLightColor"), 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.0f)));
	glUniform1f(glGetUniformLocation(simpleProgramID, "ambientLightIntensity"), (float)0.1f);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, boxTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, boxNormalTex);

	glBindVertexArray(boxVAO);
	glDrawElements(GL_TRIANGLES, boxNumIndices, GL_UNSIGNED_INT, 0);
}


int init(GLFWwindow* &window)
{
	glfwInit();

	// Tell GLFW which profile & openGL version we're using
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	// Create Window
	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Orange", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to Create window" << std::endl;
		return -1;
	}

	// Register callbacks
	glfwSetCursorPosCallback(window, cursorPosCallback);
	glfwSetKeyCallback(window, keyCallBack);

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
	float x = (float)xPos;
	float y = (float)yPos;

	if (camIsFirstMove)
	{
		camLastX = x;
		camLastY = y;
		camIsFirstMove = false;
	}

	float deltaX = x - camLastX;
	float deltaY = y - camLastY;

	camLastX = x;
	camLastY = y;

	camYaw -= deltaX;
	camPitch += deltaY;

	camPitch = glm::clamp(camPitch, -85.0f, 85.0f);

	if (camYaw > 180.0f) { camYaw -= 360.0f; }
	if (camYaw < -180.0f) { camYaw += 360.0f; }

	camQuat = glm::quat(glm::vec3(glm::radians(camPitch), glm::radians(camYaw), 0.0f));

	cameraForward = camQuat * glm::vec3(0, 0, 1);
	cameraUp = camQuat * glm::vec3(0, 1, 0);
}

void keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		keys[key] = true;
	}
	else if (action == GLFW_RELEASE)
	{
		keys[key] = false;
	}
}

void processInput(GLFWwindow*& window)
{
	if (keys[GLFW_KEY_ESCAPE])
	{
		glfwSetWindowShouldClose(window, true);
	}

	float speed = 10.0f;
	if (keys[GLFW_KEY_W])
	{
		cameraPosition += camQuat * glm::vec3(0, 0, 1 * speed);
	}
	if (keys[GLFW_KEY_S])
	{
		cameraPosition += camQuat * glm::vec3(0, 0, -1 * speed);
	}
	if (keys[GLFW_KEY_A])
	{
		cameraPosition += camQuat * glm::vec3(1 * speed, 0, 0);
	}
	if (keys[GLFW_KEY_D])
	{
		cameraPosition += camQuat * glm::vec3(-1 * speed, 0, 0);
	}
}

GLuint GeneratePlane(const char* heightmap, GLenum format, int comp, float hScale, float xzScale, unsigned int& indexCount, unsigned int& heightmapID)
{
	int width, height, channels;
	unsigned char* data = nullptr;
	if (heightmap != nullptr)
	{
		data = stbi_load(heightmap, &width, &height, &channels, comp);
		if (data)
		{
			glGenTextures(1, &heightmapID);
			glBindTexture(GL_TEXTURE_2D, heightmapID);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}

	int stride = 8;
	float* vertices = new float[(width * height) * stride];
	unsigned int* indices = new unsigned int[(width - 1) * (height - 1) * 6];

	int index = 0;
	for (int i = 0; i < (width * height); i++)
	{
		// TODO: calculate x/z values
		int x = i % width;
		int z = i / width;

		// TODO: set position
		vertices[index++] = x * xzScale;
		vertices[index++] = 0;
		vertices[index++] = z * xzScale;

		// TODO: set normal
		vertices[index++] = 0;
		vertices[index++] = 1;
		vertices[index++] = 0;

		// TODO: set uv
		vertices[index++] = x / (float)width;
		vertices[index++] = z / (float)height;
	}

	// OPTIONAL TODO: Calculate normal
	// TODO: Set normal

	index = 0;
	for (int i = 0; i < (width - 1) * (height - 1); i++)
	{
		int x = i % (width - 1);
		int z = i / (width - 1);

		int vertex = z * width + x;

		indices[index++] = vertex;
		indices[index++] = vertex + width;
		indices[index++] = vertex + width + 1;

		indices[index++] = vertex;
		indices[index++] = vertex + width + 1;
		indices[index++] = vertex + 1;
	}

	unsigned int vertSize = (width * height) * stride * sizeof(float);
	indexCount = ((width - 1) * (height - 1) * 6);

	unsigned int VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertSize, vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	// vertex information!
	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * stride, 0);
	glEnableVertexAttribArray(0);
	// normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * stride, (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);
	// uv
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * stride, (void*)(sizeof(float) * 6));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	delete[] vertices;
	delete[] indices;

	stbi_image_free(data);

	return VAO;
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

void createShaders()
{
	createProgram(simpleProgramID, "shaders/simpleVertex.glsl", "shaders/simpleFragment.glsl");
	createProgram(skyProgramID, "shaders/skyVertex.glsl", "shaders/skyFragment.glsl");
	createProgram(terrainProgramID, "shaders/terrainVertex.glsl", "shaders/terrainFragment.glsl");
}

void createProgram(GLuint &programID, const char* vertexShaderPath, const char* fragmentShaderPath)
{
	char* vertexSource;
	char* fragmentSource;

	loadFile(vertexShaderPath, vertexSource);
	loadFile(fragmentShaderPath, fragmentSource);

	int succes;
	char infoLog[512];

	GLuint vertexShaderID, framgentShaderID;

	vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderID, 1, &vertexSource, nullptr);
	glCompileShader(vertexShaderID);

	glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &succes);
	if (!succes)
	{
		glGetShaderInfoLog(vertexShaderID, 512, nullptr, infoLog);
		std::cout << "ERROR Compiling vertex shader" << infoLog << std::endl;
	}

	framgentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(framgentShaderID, 1, &fragmentSource, nullptr);
	glCompileShader(framgentShaderID);


	glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &succes);
	if (!succes)
	{
		glGetShaderInfoLog(vertexShaderID, 512, nullptr, infoLog);
		std::cout << "ERROR Compiling fragment shader" << infoLog << std::endl;
	}

	programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, framgentShaderID);
	glLinkProgram(programID);

	glGetProgramiv(programID, GL_LINK_STATUS, &succes);
	if (!succes)
	{
		glGetProgramInfoLog(programID, 512, nullptr, infoLog);
		std::cout << "ERROR Linking shader program" << infoLog << std::endl;
	}

	glDeleteShader(vertexShaderID);
	glDeleteShader(framgentShaderID);

	delete vertexSource;
	delete fragmentSource;
}

void loadFile(const char* filename, char*& output)
{
	std::ifstream stream(filename, std::ios::binary);

	if (stream.is_open())
	{
		stream.seekg(0, stream.end);
		int length = stream.tellg();
		stream.seekg(0, stream.beg);

		output = new char[length + 1];
		stream.read(output, length);

		stream.read(output, length);
		output[length] = '\0';

		stream.close();
	}
	else
	{
		output = NULL;
	}
}

GLuint loadTexture(const char* path)
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, numChannels;
	unsigned char* data = stbi_load(path, &width, &height, &numChannels, 0);

	if (data)
	{
		if (numChannels == 3)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else if (numChannels == 4)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}

		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Error loading texture: " << path << std::endl;
	}

	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);

	return textureID;
}