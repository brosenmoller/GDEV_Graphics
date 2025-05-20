#pragma once

#include <string>
#include <glad/glad.h>

struct MaterialTexture {
    GLuint id;
    std::string type;
    std::string path;
    MaterialTexture(const char* path, const char* type);
    GLuint loadTexture(const char* path, int comp = 0);
};