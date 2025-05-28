#pragma once

#include <vector>
#include "MaterialTexture.hpp"

class Material {
public:
    GLuint shaderProgram;
    std::vector<MaterialTexture> textures;

    Material(const char* vertexShaderPath, const char* fragmentShaderPath);
    Material(std::string vertexShaderPath, std::string fragmentShaderPath);
    static void createProgram(GLuint& programID, const char* vertexShaderPath, const char* fragmentShaderPath);

    void Use() const;
};