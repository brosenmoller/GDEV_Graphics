#include "Input.hpp"

bool Input::keys[1024] = { false };
glm::vec2 Input::mousePos = glm::vec2(0, 0);
glm::vec2 Input::mouseDelta = glm::vec2(0, 0);
