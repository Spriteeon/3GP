#include "ModelTerrain.h"
#include "ImageLoader.h"

ModelTerrain::ModelTerrain(float size, int numCellsXZ) : Model("", 0, 0, 0, 1.0f)
{

	m_size = size; //Set Terrain size
	m_numCellsXZ = numCellsXZ; //Set number of terrain cells

}

bool ModelTerrain::Initialise() 
{

	MyMesh terrainMesh; //Create Terrain mesh

	float cellSize = m_size / m_numCellsXZ; //Calculate cell size

	//Calculate number of vertices
	int numVertsX = m_numCellsXZ + 1;
	int numVertsZ = m_numCellsXZ + 1;

	glm::vec3 start(-m_size / 2, 0, m_size / 2); //vertex start position

	float tiles{ 10.0f }; //How many texture tiles on the terrain

	Helpers::ImageLoader heightImage;
	if (!heightImage.Load("Data/Textures/curvy.bmp")) //Load terrain heightmap
	{
		return false;
	}

	unsigned char* texels = (unsigned char*)heightImage.GetData(); //Get heightmap data

	for (int z = 0; z < numVertsZ; z++) //Loop through Z vertices
	{
		for (int x = 0; x < numVertsX; x++) //Loop through X vertices
		{

			glm::vec3 pos{ start }; //Set start position

			pos.x += x * cellSize; //Increase X and Z coords gradually
			pos.z -= z * cellSize;

			float u = (float)x / (numVertsX - 1); //Set UV coords for texture
			float v = (float)z / (numVertsZ - 1);

			int heightMapX = (int)(u * (heightImage.Width() - 1));
			int heightMapY = (int)(v * (heightImage.Height() - 1));

			int offset = (heightMapX + heightMapY * heightImage.Width()) * 4; //Set height of terrain
			pos.y = texels[offset] * 2; //Edit number to make terrain more "extreme"

			vertices.push_back(pos);

			u *= tiles; //Alter to correct UV coords
			v *= tiles;

			uvCoords.push_back(glm::vec2(u, v));

		}
	}

	std::vector<glm::uint> elements;

	bool toggleForDiamondPattern = true;

	//Create Diamond pattern for terrain mesh
	for (int cellZ = 0; cellZ < m_numCellsXZ; cellZ++)
	{
		for (int cellX = 0; cellX < m_numCellsXZ; cellX++)
		{

			int startVertIndex = cellZ * numVertsX + cellX;

			if (toggleForDiamondPattern) //First Cell triangle orientation
			{
				elements.push_back(startVertIndex);
				elements.push_back(startVertIndex + 1);
				elements.push_back(startVertIndex + numVertsX);

				elements.push_back(startVertIndex + 1);
				elements.push_back(startVertIndex + numVertsX + 1);
				elements.push_back(startVertIndex + numVertsX);
			}
			else //Second Cell triangle orientation
			{

				elements.push_back(startVertIndex);
				elements.push_back(startVertIndex + 1);
				elements.push_back(startVertIndex + numVertsX + 1);

				elements.push_back(startVertIndex);
				elements.push_back(startVertIndex + numVertsX + 1);
				elements.push_back(startVertIndex + numVertsX);

			}

			toggleForDiamondPattern = !toggleForDiamondPattern;

		}

		toggleForDiamondPattern = !toggleForDiamondPattern;
	}

	std::vector<glm::vec3> normals(vertices.size()); //Create normals vector
	std::fill(normals.begin(), normals.end(), glm::vec3(0, 0, 0));

	for (size_t i = 0; i < elements.size(); i += 3) //Normals
	{

		int index1 = elements[i];
		int index2 = elements[i + 1];
		int index3 = elements[i + 2];

		glm::vec3 v0 = vertices[index1];
		glm::vec3 v1 = vertices[index2];
		glm::vec3 v2 = vertices[index3];

		glm::vec3 edge1 = v1 - v0;
		glm::vec3 edge2 = v2 - v0;

		glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

		normals[index1] += normal;
		normals[index2] += normal;
		normals[index3] += normal;

	}

	for (glm::vec3& n : normals) //Normalise all normals for each vertex
	{
		n = glm::normalize(n);
	}

	GLuint PositionsVBO; //Positions VBO
	glGenBuffers(1, &PositionsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, PositionsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)* vertices.size(), vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLuint NormalsVBO; //Normals VBO
	glGenBuffers(1, &NormalsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, NormalsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)* normals.size(), normals.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLuint CoordsVBO; //UV Coords VBO
	glGenBuffers(1, &CoordsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, CoordsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2)* uvCoords.size(), uvCoords.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLuint ElementsEBO; //Elements EBO
	glGenBuffers(1, &ElementsEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementsEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)* elements.size(), elements.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	Helpers::CheckForGLError();
	terrainMesh.numElements = elements.size();

	if (!heightImage.Load(m_textureList[0])) //Load terrain texture
	{
		return false;
	}

	//Add terrain texture to terrain mesh
	glGenTextures(1, &terrainMesh.textureID);
	glBindTexture(GL_TEXTURE_2D, terrainMesh.textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, heightImage.Width(), heightImage.Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, heightImage.GetData());
	glGenerateMipmap(GL_TEXTURE_2D);

	glGenVertexArrays(1, &terrainMesh.VAO);
	glBindVertexArray(terrainMesh.VAO);

	glBindBuffer(GL_ARRAY_BUFFER, PositionsVBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,                  // attribute 0
		3,                  // size in bytes of each item in the stream
		GL_FLOAT,           // type of the item
		GL_FALSE,           // normalized or not (advanced)
		0,                  // stride (advanced)
		(void*)0            // array buffer offset (advanced)
	);

	glBindBuffer(GL_ARRAY_BUFFER, NormalsVBO);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,                  // attribute 0
		3,                  // size in bytes of each item in the stream
		GL_FLOAT,           // type of the item
		GL_FALSE,           // normalized or not (advanced)
		0,                  // stride (advanced)
		(void*)0            // array buffer offset (advanced)
	);

	glBindBuffer(GL_ARRAY_BUFFER, CoordsVBO);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
		2,                  // attribute 0
		2,                  // size in bytes of each item in the stream
		GL_FLOAT,           // type of the item
		GL_FALSE,           // normalized or not (advanced)
		0,                  // stride (advanced)
		(void*)0            // array buffer offset (advanced)
	);

	Helpers::CheckForGLError();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementsEBO);
	glBindVertexArray(0);

	myMeshVector.push_back(terrainMesh);

	// Clear VAO binding
	glBindVertexArray(0);

	return true;

}

float ModelTerrain::GetHeight(float posX, float posZ) //Returns height of terrain given specific X and Z values
{
	for (auto& vertex : vertices) //Loops through all vertices in terrain
	{
		if ((vertex.x - posX) < 0.1 || (vertex.x - posX) > -0.1)
		{
			if((vertex.z - posZ) < 0.1 || (vertex.z - posZ) > -0.1)
			{

				//std::cout << vertex.y << std::endl;
				return vertex.y;
			
			}
		}

	}
}

