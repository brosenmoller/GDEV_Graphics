#include "Scene.hpp"
#include <fstream>
#include "../json.hpp"
#include "../Objects/Camera.hpp"
#include "../rendering/SkyBox.hpp"
#include "../core/Debug.hpp"

using json = nlohmann::json;

Scene::Scene(std::string sceneDataPath)
{
	std::ifstream file(sceneDataPath);
	if (!file.is_open()) {
		throw std::runtime_error("Failed to open scene file: " + sceneDataPath);
	}

	json j;
	file >> j;

	sceneData.cameraPosition = j.contains("cameraPosition")
		? glm::vec3(j["cameraPosition"][0], j["cameraPosition"][1], j["cameraPosition"][2])
		: glm::vec3(0, 0, 0);

	sceneData.cameraRotation = j.contains("cameraRotation")
		? glm::vec3(j["cameraRotation"][0], j["cameraRotation"][1], j["cameraRotation"][2])
		: glm::vec3(0, 0, 0);

	sceneData.lightDirection = j.contains("lightDirection")
		? glm::vec3(j["lightDirection"][0], j["lightDirection"][1], j["lightDirection"][2])
		: glm::normalize(glm::vec3(0.0f, -0.5f, -0.5f));

	sceneData.skyBox.topColor = j.contains("skyBox_topColor")
		? glm::vec3(j["skyBox_topColor"][0], j["skyBox_topColor"][1], j["skyBox_topColor"][2])
		: glm::vec3(68.0f, 118.0f, 189.0f);

	sceneData.skyBox.middleColor = j.contains("skyBox_middleColor")
		? glm::vec3(j["skyBox_middleColor"][0], j["skyBox_middleColor"][1], j["skyBox_middleColor"][2])
		: glm::vec3(188.0f, 214.0f, 231.0f);

	sceneData.skyBox.bottomColor = j.contains("skyBox_bottomColor")
		? glm::vec3(j["skyBox_bottomColor"][0], j["skyBox_bottomColor"][1], j["skyBox_bottomColor"][2])
		: glm::vec3(20.0f, 20.0f, 21.0f);

	sceneData.skyBox.sunColor = j.contains("skyBox_sunColor")
		? glm::vec3(j["skyBox_sunColor"][0], j["skyBox_sunColor"][1], j["skyBox_sunColor"][2])
		: glm::vec3(255.0f, 200.0f, 50.0f);

	for (std::size_t i = 0; i < j["materials"].size(); ++i) {
		const json& m = j["materials"][i];
		MaterialData mat;
		mat.materialKey = m["materialKey"];
		mat.vertexShaderPath = m["vertexShaderPath"];
		mat.fragmentShaderPath = m["fragmentShaderPath"];
		sceneData.materials.push_back(mat);
	}

	for (std::size_t i = 0; i < j["models"].size(); ++i) {
		const json& m = j["models"][i];
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

		obj.position = o.contains("position")
			? glm::vec3(o["position"][0], o["position"][1], o["position"][2])
			: glm::vec3(0.0f, 0.0f, 0.0f);

		obj.rotation = o.contains("rotation")
			? glm::vec3(o["rotation"][0], o["rotation"][1], o["rotation"][2])
			: glm::vec3(0.0f, 0.0f, 0.0f);

		obj.scale = o.contains("scale")
			? glm::vec3(o["scale"][0], o["scale"][1], o["scale"][2])
			: glm::vec3(1.0f, 1.0f, 1.0f);

		sceneData.objects.push_back(obj);
	}
}

void Scene::UpdateScene()
{
	process();
	draw();
}

void Scene::CreateScene()
{
	Camera::destroy();
	Camera::init(sceneData.lightDirection, sceneData.cameraPosition, sceneData.cameraRotation);

	skybox = std::make_shared<SkyBox>();
	updateables.push_back(skybox);

	for (std::size_t i = 0; i < sceneData.materials.size(); i++) 
	{
		materialPointers.insert({ 
			sceneData.materials[i].materialKey, 
			std::make_unique<Material>(sceneData.materials[i].vertexShaderPath, sceneData.materials[i].fragmentShaderPath)
		});
	}

	for (std::size_t i = 0; i < sceneData.models.size(); i++)
	{
		modelPointers.insert({ 
			sceneData.models[i].modelKey, 
			std::make_unique<Model>(sceneData.models[i].modelPath) 
		});
	}

	for (std::size_t i = 0; i < sceneData.objects.size(); i++)
	{
		const ObjectData objectData = sceneData.objects[i];
		addRenderObject(objectData.modelKey, objectData.materialKey, objectData.position, objectData.rotation, objectData.scale);
	}
}

void Scene::DeleteScene()
{
	materialPointers.clear();
	modelPointers.clear();
	updateables.clear();
	renderObjects.clear();
}

void Scene::process()
{
	Camera::Instance()->Update();
	for (const auto& updateable : updateables) {
		updateable->Update();
	}
}

void Scene::draw()
{
	for (const auto& renderObject : renderObjects) {
		renderObject->DrawObject();
	}
}

void Scene::addRenderObject(std::string modelKey, std::string materialKey, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
	std::shared_ptr<RenderObject> renderObject = std::make_shared<RenderObject>(
		modelPointers[modelKey].get(), materialPointers[materialKey].get(), position, glm::quat(rotation), scale
	);

	updateables.push_back(renderObject);
	renderObjects.push_back(renderObject);
}