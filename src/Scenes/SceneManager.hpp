#pragma once

#include "Scene.hpp"
#include <map>

class SceneManager 
{
public:
	std::string activeScene;
	std::map<std::string, std::unique_ptr<Scene>> scenes;

	SceneManager(std::string sceneDirectoryPath, std::string firstScene = "");
	void LoadScene(std::string sceneName);
	void UpdateScene();
};