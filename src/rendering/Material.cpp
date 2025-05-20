#include "Material.hpp"
#include "../core/File.hpp"
#include <iostream>
#include "../core/Debug.hpp"

GLuint Material::lastShaderProgramID = 0;

Material::Material(const char* vertexShaderPath, const char* fragmentShaderPath)
{
	shaderProgram = lastShaderProgramID;
	lastShaderProgramID++;

    createProgram(shaderProgram, vertexShaderPath, fragmentShaderPath);
    glUseProgram(shaderProgram);

    glUniform1i(glGetUniformLocation(shaderProgram, "texture_diffuse1"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_specular1"), 1);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_normal1"), 2);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_roughness1"), 3);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_ao1"), 4);
}

void Material::createProgram(GLuint& programID, const char* vertexShaderPath, const char* fragmentShaderPath)
{
	char* vertexSource;
	char* fragmentSource;

	File::LoadFile(vertexShaderPath, vertexSource);
	File::LoadFile(fragmentShaderPath, fragmentSource);

	int succes;
	char infoLog[512];

	Debug::Log("Create Program");

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

void Material::Use() const {
    glUseProgram(shaderProgram);

    //for (GLint i = 0; i < textures.size(); ++i) {
    //    glActiveTexture(GL_TEXTURE0 + i);
    //    glBindTexture(GL_TEXTURE_2D, textures[i].id);
    //    glUniform1i(glGetUniformLocation(shaderProgram, textures[i].type.c_str()), i);
    //}
}