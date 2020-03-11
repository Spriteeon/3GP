#pragma once

#include "Helper.h"
#include "ExternalLibraryHeaders.h"
#include "Camera.h"

struct MyMesh //Mesh Structure
{

	GLuint VAO;
	GLuint numElements;
	GLuint textureID{ 0 };

};

class Model
{

protected:

	std::string modelName; //Name of Model
	std::string textureName; //Name of Texture 
	 
	//Vectors for Meshes and Texture file 
	std::vector<MyMesh> myMeshVector;
	std::vector<std::string> m_textureList;

	float m_posX{ 0 }, m_posY{ 0 }, m_posZ{ 0 }, m_scale{ 0 }; //Set initial positions for Model

public:

	Model(const std::string& name, const float& posX, const float& posY, const float& posZ, const float& scale);
	~Model();

	virtual bool Initialise();
	virtual void Render(const Helpers::Camera& camera, GLuint m_program, glm::mat4& projection_xform, glm::mat4& view_xform);

	float GetXPos() { return m_posX; }; //Returns Model X position
	float GetZPos() { return m_posZ; }; //Returns Model Z position
	virtual float GetHeight(float posX, float posZ) { return 0; };

	void Texture(const std::string& filename);

	void Move(const float& x, const float& y, const float& z);

};

