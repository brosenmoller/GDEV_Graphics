#pragma once

#include <glad/glad.h>
#include <string>

class Material {
public:
    GLuint shaderProgram;

    Material(const char* vertexShaderPath, const char* fragmentShaderPath);
    Material(std::string vertexShaderPath, std::string fragmentShaderPath);
    static void createProgram(GLuint& programID, const char* vertexShaderPath, const char* fragmentShaderPath);

    void Use() const;
};