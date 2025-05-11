#pragma once

#include "Object.hpp"
#include <memory>
#include <mutex>

class Camera : public Object 
{
public:
	static Camera* Instance() {
		if (!instance_) {
			throw std::runtime_error("Singleton not initialized");
		}
		return instance_.get();
	}

	static void init(glm::vec3 lightDirection, glm::vec3 position, glm::quat rotation = glm::quat(glm::vec3(0, 0, 0)), glm::vec3 scale = glm::vec3(1, 1, 1)) {
		std::call_once(initFlag_, [&]() {
			instance_ = std::unique_ptr<Camera>(new Camera(lightDirection, position, rotation, scale));
		});
	}

	Camera(const Camera&) = delete;
	Camera& operator=(const Camera&) = delete;

	glm::vec3 cameraForward = glm::vec3(0, 0, 1);
	glm::vec3 cameraUp = glm::vec3(0, 1, 0);
	glm::vec2 camLast = glm::vec2(0, 0);

	glm::mat4 view;
	glm::mat4 projection;
	glm::vec3 lightDirection;

	float camYaw, camPitch;

	Camera(glm::vec3 lightDirection, glm::vec3 position, glm::quat rotation = glm::quat(glm::vec3(0, 0, 0)), glm::vec3 scale = glm::vec3(1, 1, 1));
	void Update();

private:
	static std::unique_ptr<Camera> instance_;
	static std::once_flag initFlag_;
};