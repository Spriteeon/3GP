#pragma once
#include "Model.h"

class ModelTerrain : public Model
{

private:

	float m_size{ 0 };
	int m_numCellsXZ{ 0 };

	std::vector<glm::vec3> vertices; //Vertex vector
	std::vector<glm::vec2> uvCoords; //UV coords vector

public:

	ModelTerrain(float size, int numCellsXZ);
	bool Initialise() override final;

	float GetHeight(float posX, float posZ) override final;

};

