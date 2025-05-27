#pragma once
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vector>

class ObjectData 
{
public:
	std::string modelKey;
	std::string materialKey;
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};

class ModelData 
{
public:
	std::string modelKey;
	std::string modelPath;
};

class MaterialData 
{
public:
	std::string materialKey;
	std::string vertexShaderPath;
	std::string fragmentShaderPath;
};

class SceneData 
{
public:
	glm::vec3 cameraPosition;
	glm::vec3 cameraRotation;
	std::vector<MaterialData> materials;
	std::vector<ModelData> models;
	std::vector<ObjectData> objects;
};