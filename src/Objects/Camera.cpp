#include "Camera.hpp"
#include "../core/Input.hpp"
#include "../core/constants.hpp"
#include "../core/Debug.hpp"

std::unique_ptr<Camera> Camera::instance_ = nullptr;
std::mutex Camera::initMutex_;

Camera::Camera(glm::vec3 lightDirection, glm::vec3 position, float pitchDeg, float yawDeg) 
	: lightDirection(lightDirection),
	camPitch(pitchDeg),
	camYaw(yawDeg),
	Object(position, glm::quat(glm::vec3(glm::radians(pitchDeg), glm::radians(yawDeg), 0.0f)))
{
	cameraForward = rotation * glm::vec3(0, 0, 1);
	cameraUp = rotation * glm::vec3(0, 1, 0);

	projection = glm::perspective(glm::radians(50.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 5000.0f);
	view = glm::lookAt(position, position + cameraForward, cameraUp);
	firstMouseFrame = true;
}

void Camera::Update()
{
	UpdateCameraMovement();
	UpdateCameraLook();

	view = glm::lookAt(position, position + cameraForward, cameraUp);
}

void Camera::UpdateCameraMovement()
{
	float speed;
	if (Input::keys[GLFW_KEY_LEFT_SHIFT])
	{
		speed = 0.5f;
	}
	else
	{
		speed = 0.05f;
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

void Camera::UpdateCameraLook()
{
	if (glm::length(Input::mouseDelta) < 0.01f) { return; }

	if (firstMouseFrame)
	{
		Input::mouseDelta = glm::vec2(0.0f);
		firstMouseFrame = false;
		return;
	}

	camYaw -= Input::mouseDelta.x * 0.5f;
	camPitch += Input::mouseDelta.y * 0.2f;

	camPitch = glm::clamp(camPitch, -85.0f, 85.0f);

	if (camYaw > 180.0f) { camYaw -= 360.0f; }
	if (camYaw < -180.0f) { camYaw += 360.0f; }

	rotation = glm::quat(glm::vec3(glm::radians(camPitch), glm::radians(camYaw), 0.0f));

	cameraForward = rotation * glm::vec3(0, 0, 1);
	cameraUp = rotation * glm::vec3(0, 1, 0);

	Input::mouseDelta = glm::vec2(0.0f);
}
