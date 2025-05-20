#pragma once
#include <glad/glad.h>
#include "../Objects/IUpdate.hpp"

class SkyBox : public IUpdate
{
private:
	GLuint skyProgramID;
	GLuint boxVAO;
	int boxNumVertices;
	int boxNumIndices;
	void createCubeMesh();

public:
	SkyBox();
	void Update();
};