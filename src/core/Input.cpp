#include "Input.hpp"

bool Input::keys[1024] = { false };
glm::vec2 Input::mousePos = glm::vec2(0, 0);
glm::vec2 Input::mouseDelta = glm::vec2(0, 0);
glm::vec2 Input::lastMousePos = glm::vec2(0, 0);

void Input::CursorPosCallback(GLFWwindow* window, double xPos, double yPos)
{
	Input::lastMousePos = Input::mousePos;
	Input::mousePos.x = (float)xPos;
	Input::mousePos.y = (float)yPos;
	Input::mouseDelta = Input::mousePos - Input::lastMousePos;
}

void Input::KeyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		Input::keys[key] = true;
	}
	else if (action == GLFW_RELEASE)
	{
		Input::keys[key] = false;
	}
}