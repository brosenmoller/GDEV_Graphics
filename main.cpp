#include <iostream>
#include <fstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

int init(GLFWwindow* &window);
void processInput(GLFWwindow*& window);
void createTriangle(GLuint& VAO, int& numVertices, int& numIndices);
void createShaders();
void createProgram(GLuint& programID, const char* vertexShaderPath, const char* fragmentShaderPath);
void loadFile(const char* filename, char*& output);

// Program IDs
GLuint simpleProgramID;

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

int main()
{
	GLFWwindow* window;
	int result = init(window);
	if (result != 0) { return result; }

	GLuint triangleVAO;
	int numVertices;
	int numIndices;
	createTriangle(triangleVAO, numVertices, numIndices);
	createShaders();

	// Create Viewport
	glViewport(0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT);


	// Game render loop
	while (!glfwWindowShouldClose(window))
	{
		// input handling
		processInput(window);
		// rendering

		// background color set & render
		glClearColor(0.1f, 0.1f, 0.1f, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(simpleProgramID);
		glBindVertexArray(triangleVAO);
		glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);

		// event polls
		glfwPollEvents();
	}


	// terminate
	return 0;
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

void processInput(GLFWwindow*& window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}


void createTriangle(GLuint& VAO, int& numVertices, int& numIndices)  //VAO = Vertex Array Object
{
	float quadPixelHeight = 100;
	float quadPixelWidth = 100;
	float quadNormalizedHeight = quadPixelHeight / (SCREEN_HEIGHT / 2.0f);
	float quadNormalizedWidth = quadPixelWidth / (SCREEN_WIDTH / 2.0f);

	float vertices[] = {
		// position												// color
		-quadNormalizedWidth, quadNormalizedHeight, 0.0f, 		1.0f, 0.0f, 0.0f, 1.0f, // Top Left
		quadNormalizedWidth, quadNormalizedHeight, 0.0f,		0.0f, 1.0f, 0.0f, 1.0f, // Top Right
		quadNormalizedWidth, -quadNormalizedHeight, 0.0f,		0.0f, 0.0f, 1.0, 1.0f, // Bottom Right
		-quadNormalizedWidth, -quadNormalizedHeight, 0.0f,		1.0f, 1.0f, 1.0f, 1.0f, // Bottom Left
	};

	int indices[] = {
		0, 1, 2,  // first triangle
		0, 2, 3   // second triangle
	};

	int stride = (3 + 4) * sizeof(float);
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

	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

void createShaders()
{
	createProgram(simpleProgramID, "shaders/simpleVertex.glsl", "shaders/simpleFragment.glsl");
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