#pragma once

#include "IUpdate.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

class Object : public IUpdate
{
public:
	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;
	glm::mat4 CalculateTransform() const;
	Object(glm::vec3 position, glm::quat rotation = glm::quat(glm::vec3(0, 0, 0)), glm::vec3 scale = glm::vec3(1, 1, 1));
};