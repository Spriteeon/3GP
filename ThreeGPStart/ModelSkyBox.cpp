#include "ModelSkyBox.h"
#include "Helper.h"
#include "Mesh.h"
#include "ImageLoader.h"

ModelSkyBox::ModelSkyBox(const std::string& filename) : Model(filename, 0, 0, 0, 1)
{

}

bool ModelSkyBox::Initialise()
{

	Helpers::ModelLoader loader;
	Helpers::ImageLoader imageLoader;

	int counter = 0; //start counter at 0

	if (!loader.LoadFromFile(modelName)) //Load Model
	{
		return false;
	}

	for (const Helpers::Mesh& mesh : loader.GetMeshVector()) //Loop through all meshes in model
	{

		MyMesh skyBoxMesh;

		//Create VBOs
		GLuint positionsVBO; //Positions VBO
		glGenBuffers(1, &positionsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, positionsVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.vertices.size(), mesh.vertices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		GLuint normalsVBO; //Normals VBO
		glGenBuffers(1, &normalsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.normals.size(), mesh.normals.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		GLuint texcoordsVBO; //UV coords VBO
		glGenBuffers(1, &texcoordsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, texcoordsVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * mesh.uvCoords.size(), mesh.uvCoords.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		GLuint elementsEBO; //Elements EBO
		glGenBuffers(1, &elementsEBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * mesh.elements.size(), mesh.elements.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		skyBoxMesh.numElements = mesh.elements.size();

		if (!imageLoader.Load("Data\\Sky\\Clouds\\" + loader.GetMaterialVector()[mesh.materialIndex].diffuseTextureFilename)) //Load SKybox textures
		{
			return false;
		}

		counter++; //Add one to counter

		//Add Skybox textures to skybox mesh
		glGenTextures(1, &skyBoxMesh.textureID);
		glBindTexture(GL_TEXTURE_2D, skyBoxMesh.textureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageLoader.Width(), imageLoader.Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, imageLoader.GetData());
		glGenerateMipmap(GL_TEXTURE_2D);

		//Create VAOs
		glGenVertexArrays(1, &skyBoxMesh.VAO);
		glBindVertexArray(skyBoxMesh.VAO);

		// Bind the vertex buffer to the context (records this action in the VAO)
		glBindBuffer(GL_ARRAY_BUFFER, positionsVBO);

		// Enable the first attribute in the program (the vertices) to stream to the vertex shader
		glEnableVertexAttribArray(0);

		// Describe the make up of the vertex stream
		glVertexAttribPointer(
			0,                  // attribute 0
			3,                  // size in bytes of each item in the stream
			GL_FLOAT,           // type of the item
			GL_FALSE,           // normalized or not (advanced)
			0,                  // stride (advanced)
			(void*)0            // array buffer offset (advanced)
		);

		glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
		// Enable the normal
		glEnableVertexAttribArray(1);

		// Describe the make up of the vertex stream
		glVertexAttribPointer(
			1,                  // attribute 1
			3,                  // size in bytes of each item in the stream
			GL_FLOAT,           // type of the item
			GL_FALSE,           // normalized or not (advanced)
			0,                  // stride (advanced)
			(void*)0            // array buffer offset (advanced)
		);

		glBindBuffer(GL_ARRAY_BUFFER, texcoordsVBO);
		// Enable the normal
		glEnableVertexAttribArray(2);

		// Describe the make up of the vertex stream
		glVertexAttribPointer(
			2,                  // attribute 1
			2,                  // size in bytes of each item in the stream
			GL_FLOAT,           // type of the item
			GL_FALSE,           // normalized or not (advanced)
			0,                  // stride (advanced)
			(void*)0            // array buffer offset (advanced)
		);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsEBO);

		// Clear VAO binding
		glBindVertexArray(0);

		myMeshVector.push_back(skyBoxMesh);

	}

	return true;

}

void ModelSkyBox::Render(const Helpers::Camera& camera, GLuint m_program, glm::mat4& projection_xform, glm::mat4& view_xform)
{

	for (const auto& mesh : myMeshVector) //For every Mesh
	{

		glm::mat4 view_xform2 = glm::mat4(glm::mat3(view_xform));
		glm::mat4 combined_xform = projection_xform * view_xform2;

		glDepthMask(GL_FALSE);
		glDisable(GL_DEPTH_TEST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// Send the combined matrix to the shader in a uniform
		GLuint combined_xform_id = glGetUniformLocation(m_program, "combined_xform");
		glUniformMatrix4fv(combined_xform_id, 1, GL_FALSE, glm::value_ptr(combined_xform));

		glm::mat4 model_xform(1);

		// Send the model matrix to the shader in a uniform
		GLuint model_xform_id = glGetUniformLocation(m_program, "model_xform");
		glUniformMatrix4fv(model_xform_id, 1, GL_FALSE, glm::value_ptr(model_xform));

		if (mesh.textureID) //Error Catching
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, mesh.textureID);
			glUniform1i(glGetUniformLocation(m_program, "sampler_tex"), 0);
		}
		else
		{
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		glBindVertexArray(mesh.VAO);
		glDrawElements(GL_TRIANGLES, mesh.numElements, GL_UNSIGNED_INT, (void*)0);

		Helpers::CheckForGLError();

		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);

	}

}
