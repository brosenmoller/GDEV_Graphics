#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include "../rendering/model.hpp"
#include "../rendering/Material.hpp"
#include "Object.hpp"

class RenderObject : public Object
{
public:
	Material* material;
	RenderObject(Model* model, Material* material, glm::vec3 position, glm::quat rotation = glm::quat(glm::vec3(0, 0, 0)), glm::vec3 scale = glm::vec3(1, 1, 1));
	void DrawObject() const;

private:
	Model* model;
};