#include "RenderObject.hpp"
#include "Camera.hpp"
#include "../core/Debug.hpp"

RenderObject::RenderObject(Model* model, Material* material, glm::vec3 position, glm::quat rotation, glm::vec3 scale)
	: Object(position, rotation, scale), material(material), model(model) { }

void RenderObject::DrawObject() const
{
	material->Use();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_DEPTH);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glUniformMatrix4fv(glGetUniformLocation(material->shaderProgram, "transform"), 1, GL_FALSE, glm::value_ptr(RenderObject::CalculateTransform()));
	glUniformMatrix4fv(glGetUniformLocation(material->shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(Camera::Instance()->view));
	glUniformMatrix4fv(glGetUniformLocation(material->shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(Camera::Instance()->projection));

	glUniform3fv(glGetUniformLocation(material->shaderProgram, "cameraPosition"), 1, glm::value_ptr(Camera::Instance()->position));
	glUniform3fv(glGetUniformLocation(material->shaderProgram, "lightDirection"), 1, glm::value_ptr(Camera::Instance()->lightDirection));

	model->Draw(material->shaderProgram);

	glDisable(GL_BLEND);
}