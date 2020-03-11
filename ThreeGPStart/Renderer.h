#pragma once

#include "ExternalLibraryHeaders.h"

#include "Helper.h"
#include "Mesh.h"
#include "Camera.h"

class Model;
class Renderer
{
protected:
	// Program object - to host shaders
	GLuint m_program{ 0 };
	// Vertex Array Object to wrap all render settings
	GLuint m_VAO{ 0 };
	// Number of elments to use when rendering
	GLuint m_numElements{ 0 };

	bool CreateProgram();
public:

	std::vector<Model*> myModels; //Vector for all models

	Renderer()=default;
	~Renderer();

	bool InitialiseGeometry();

	// Render the scene
	void Render(const Helpers::Camera& camera, float deltaTime);
	float GetHeight(Model& model, float posX, float posZ);

};

