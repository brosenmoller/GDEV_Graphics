#include "Material.hpp"

void Material::Use() {
    glUseProgram(shaderProgram);

    for (GLint i = 0; i < textures.size(); ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
        glUniform1i(glGetUniformLocation(shaderProgram, textures[i].type.c_str()), i);
    }
}
