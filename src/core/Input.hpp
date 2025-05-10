#pragma once

#include "glm/vec2.hpp"
#include <GLFW/glfw3.h>

class Input
{
public:
	static bool keys[1024];
	static glm::vec2 lastMousePos;
	static glm::vec2 mousePos;
	static glm::vec2 mouseDelta;
	static void KeyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void CursorPosCallback(GLFWwindow* window, double xPos, double yPos);
};