#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

class Object
{
public:
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

private:
	void CalculateTransform()
	{
		glm::mat4 transform = glm::mat4(1.0f);
		transform = glm::translate(transform, position);
		transform = transform * glm::toMat4(glm::quat(glm::radians(rotation)));
		transform = glm::scale(transform, scale);
	}
};