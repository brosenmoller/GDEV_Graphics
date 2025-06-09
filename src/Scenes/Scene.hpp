#pragma once
#include <string>
#include <map>
#include "SceneData.hpp"
#include "../rendering/model.hpp"
#include "../rendering/Material.hpp"
#include "../Objects/RenderObject.hpp"
#include "../rendering/SkyBox.hpp"

class Scene
{
public:
	SceneData sceneData;
	Scene(std::string sceneDataPath);
	
	std::shared_ptr<SkyBox> skybox;
	std::map<std::string, std::unique_ptr<Model>> modelPointers;
	std::map<std::string, std::unique_ptr<Material>> materialPointers;

	void UpdateScene();
	void CreateScene();
	void DeleteScene();

private:
	std::vector<std::shared_ptr<IUpdate>> updateables;
	std::vector<std::shared_ptr<RenderObject>> renderObjects;

	void process();
	void draw();
	void addRenderObject(std::string modelKey, std::string materialKey, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);
};