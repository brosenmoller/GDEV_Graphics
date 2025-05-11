#pragma once

#include <vector>
#include "MaterialTexture.hpp"

class Material {
public:
    GLuint shaderProgram;
    std::vector<MaterialTexture> textures;

    void Use();
};