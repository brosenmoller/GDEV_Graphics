#pragma once
#include <glad/glad.h>
#include "../Objects/IUpdate.hpp"
#include "../Scenes/SceneData.hpp"

class SkyBox : public IUpdate
{
private:
	GLuint skyProgramID;
	GLuint boxVAO;
	SkyBoxData* skyboxData;
	int boxNumVertices;
	int boxNumIndices;
	void createCubeMesh();

public:
	SkyBox();
	void Update();
	void SetSkyBoxData(SkyBoxData* _skyboxData);
};