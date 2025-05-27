#include "Scene.hpp"
#include <fstream>
#include "../json.hpp"

using json = nlohmann::json;

Scene::Scene(std::string sceneDataPath)
{
	std::ifstream file(sceneDataPath);
	if (!file.is_open()) {
		throw std::runtime_error("Failed to open scene file: " + sceneDataPath);
	}

	json j;
	file >> j;

	sceneData.cameraPosition = glm::vec3(
		j["cameraPosition"][0],
		j["cameraPosition"][1],
		j["cameraPosition"][2]
	);

	sceneData.cameraRotation = glm::vec3(
		j["cameraRotation"][0],
		j["cameraRotation"][1],
		j["cameraRotation"][2]
	);

	for (std::size_t i = 0; i < j["materials"].size(); ++i) {
		const json& m = j["materials"][i];
		MaterialData mat;
		mat.materialKey = m["materialKey"];
		mat.vertexShaderPath = m["vertexShaderPath"];
		mat.fragmentShaderPath = m["fragmentShaderPath"];
		sceneData.materials.push_back(mat);
	}

	for (std::size_t i = 0; i < j["models"].size(); ++i) {
		const json& m = j["objects"][i];
		ModelData model;
		model.modelKey = m["modelKey"];
		model.modelPath = m["modelPath"];
		sceneData.models.push_back(model);
	}

	for (std::size_t i = 0; i < j["objects"].size(); ++i) {
		const json& o = j["objects"][i];
		ObjectData obj;
		obj.modelKey = o["modelKey"];
		obj.materialKey = o["materialKey"];

		if (o.contains("position")) 
		{
			obj.position = glm::vec3(o["position"][0], o["position"][1], o["position"][2]);
		}
		else { obj.position = glm::vec3(0.0f, 0.0f, 0.0f); }

		if (o.contains("rotation")) 
		{
			obj.rotation = glm::vec3(o["rotation"][0], o["rotation"][1], o["rotation"][2]);
		}
		else { obj.rotation = glm::vec3(0.0f, 0.0f, 0.0f); }

		if (o.contains("scale")) 
		{
			obj.scale = glm::vec3(o["scale"][0], o["scale"][1], o["scale"][2]);
		}
		else { obj.scale = glm::vec3(1.0f, 1.0f, 1.0f); }

		sceneData.objects.push_back(obj);
	}
}