#pragma once
#include <string>
#include "SceneData.hpp"

class Scene
{
public:
	SceneData sceneData;
	Scene(std::string sceneDataPath);
};