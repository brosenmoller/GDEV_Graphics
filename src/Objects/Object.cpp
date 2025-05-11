#include "Object.hpp"

glm::mat4 Object::CalculateTransform() const
{
	glm::mat4 transform = glm::mat4(1.0f);
	transform = glm::translate(transform, position);
	transform = transform * glm::toMat4(rotation);
	transform = glm::scale(transform, scale);
	return transform;
}

Object::Object(glm::vec3 position, glm::quat rotation, glm::vec3 scale) : position(position), rotation(rotation), scale(scale) {}
