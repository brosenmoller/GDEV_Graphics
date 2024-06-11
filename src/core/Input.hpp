#pragma once

#include "glm/vec2.hpp"

class Input
{
public:
	static bool keys[1024];
	static glm::vec2 mousePos;
	static glm::vec2 mouseDelta;
};