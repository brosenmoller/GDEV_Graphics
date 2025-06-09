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

	static void init(glm::vec3 lightDirection, glm::vec3 position, float pitchDeg, float yawDeg) {
		std::lock_guard<std::mutex> lock(initMutex_);
		if (!instance_) {
			instance_ = std::unique_ptr<Camera>(new Camera(lightDirection, position, pitchDeg, yawDeg));
		};
	}

	static void destroy() {
		std::lock_guard<std::mutex> lock(initMutex_);
		instance_.reset();
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
	bool firstMouseFrame;

	void Update();
	void UpdateCameraMovement();
	void UpdateCameraLook();

	Camera(glm::vec3 lightDirection, glm::vec3 position, float pitchDeg, float yawDeg);
	~Camera() {}
private:

	static std::unique_ptr<Camera> instance_;
	static std::mutex initMutex_;
};