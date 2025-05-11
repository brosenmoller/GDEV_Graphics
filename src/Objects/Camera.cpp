#include "Camera.hpp"
#include "../core/Input.hpp"
#include "../core/constants.hpp"

std::unique_ptr<Camera> Camera::instance_;
std::once_flag Camera::initFlag_;

Camera::Camera(glm::vec3 lightDirection, glm::vec3 position, glm::quat rotation, glm::vec3 scale)
	: lightDirection(lightDirection), Object(position, rotation, scale)
{
	projection = glm::perspective(glm::radians(50.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 5000.0f);
	camPitch = 0;
	camYaw = 0;
	view = glm::lookAt(position, position + cameraForward, cameraUp);
}

void Camera::Update()
{
	view = glm::lookAt(position, position + cameraForward, cameraUp);

	camYaw -= Input::mouseDelta.x * 0.5f;
	camPitch += Input::mouseDelta.y * 0.2f;

	camPitch = glm::clamp(camPitch, -85.0f, 85.0f);

	if (camYaw > 180.0f) { camYaw -= 360.0f; }
	if (camYaw < -180.0f) { camYaw += 360.0f; }

	rotation = glm::quat(glm::vec3(glm::radians(camPitch), glm::radians(camYaw), 0.0f));

	cameraForward = rotation * glm::vec3(0, 0, 1);
	cameraUp = rotation * glm::vec3(0, 1, 0);

	float speed;
	if (Input::keys[GLFW_KEY_LEFT_SHIFT])
	{
		speed = 50.0f;
	}
	else
	{
		speed = 5.0f;
	}

	if (Input::keys[GLFW_KEY_W])
	{
		position += rotation * glm::vec3(0, 0, 1 * speed);
	}
	if (Input::keys[GLFW_KEY_S])
	{
		position += rotation * glm::vec3(0, 0, -1 * speed);
	}
	if (Input::keys[GLFW_KEY_A])
	{
		position += rotation * glm::vec3(1 * speed, 0, 0);
	}
	if (Input::keys[GLFW_KEY_D])
	{
		position += rotation * glm::vec3(-1 * speed, 0, 0);
	}
}
