#pragma once
#include "Model.h"

class ModelSkyBox : public Model
{

private:



public:

	ModelSkyBox(const std::string& filename);

	bool Initialise() override final;
	void Render(const Helpers::Camera& camera, GLuint m_program, glm::mat4& projection_xform, glm::mat4& view_xform) override final;

};

