#pragma once

#include <string>
#include <glad/glad.h>

struct MaterialTexture {
    GLuint id;
    std::string type; // e.g., "texture_diffuse", "texture_specular"
    std::string path;
    MaterialTexture(GLuint id, std::string path, std::string type) : id(id), path(path), type(type) {};
};