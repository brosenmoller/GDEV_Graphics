#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>

class Debug 
{
public:
	static void Log(const char* message) {
		std::cout << "[DEBUG]: " << message << std::endl;
	}

	static void Log(std::string message) {
		std::cout << "[DEBUG]: " << message << std::endl;
	}

	static void Log(glm::vec2 value) { Log(glm::to_string(value)); }
	static void Log(glm::vec3 value) { Log(glm::to_string(value)); }
	static void Log(glm::mat3 value) { Log(glm::to_string(value)); }
	static void Log(glm::mat4 value) { Log(glm::to_string(value)); }
	static void Log(glm::quat value) { Log(glm::to_string(value)); }
};