#include "Mesh.h"

namespace Helpers
{
	// Replacements for KD utility lib calls
	inline void EsOutput(const std::string& what) { std::cout << what << std::endl; }
	inline void EsError(const std::string& what) { std::cerr << "Error: " << what << std::endl; }
	inline glm::vec4 aiColor4DToGlmVec4(aiColor4D col) { return glm::vec4(col.r, col.g, col.b, col.a); }
#define EsAssert assert

	// Retrieve the dimensions of this mesh in local coordinates
	void Mesh::GetLocalExtents(glm::vec3& minExtents, glm::vec3& maxExtents) const
	{
		if (vertices.empty())
			return;

		minExtents.x = maxExtents.x = vertices[0].x;
		minExtents.y = maxExtents.y = vertices[0].y;
		minExtents.z = maxExtents.z = vertices[0].z;

		for (size_t i = 1; i < vertices.size(); i++)
		{
			minExtents.x = std::min(minExtents.x, vertices[i].x);
			minExtents.y = std::min(minExtents.y, vertices[i].y);
			minExtents.z = std::min(minExtents.z, vertices[i].z);

			maxExtents.x = std::max(maxExtents.x, vertices[i].x);
			maxExtents.y = std::max(maxExtents.y, vertices[i].y);
			maxExtents.z = std::max(maxExtents.z, vertices[i].z);
		}
	}

	// Load a 3D model form a provided file and path, return false on error
	bool ModelLoader::LoadFromFile(const std::string& objFilename)
	{
		m_filename = objFilename;

		EsOutput("\nUsing assimp to load: " + objFilename);

		// Commom post processing steps - may slow load but make mesh better optimised
		unsigned int ppsteps = aiProcess_CalcTangentSpace | // calculate tangents and bitangents if possible
			aiProcess_JoinIdenticalVertices |				// join identical vertices/ optimize indexing
			aiProcess_ValidateDataStructure |				// perform a full validation of the loader's output
			aiProcess_ImproveCacheLocality |				// improve the cache locality of the output vertices
			aiProcess_RemoveRedundantMaterials |			// remove redundant materials
			aiProcess_FindDegenerates |						// remove degenerated polygons from the import
			aiProcess_FindInvalidData |						// detect invalid model data, such as invalid normal vectors
			aiProcess_GenUVCoords |							// convert spherical, cylindrical, box and planar mapping to proper UVs
			aiProcess_TransformUVCoords |					// preprocess UV transformations (scaling, translation ...)
			aiProcess_FindInstances |						// search for instanced meshes and remove them by references to one master
			aiProcess_LimitBoneWeights |					// limit bone weights to 4 per vertex
			aiProcess_OptimizeMeshes |						// join small meshes, if possible;
			aiProcess_SplitByBoneCount |					// split meshes with too many bones.
			aiProcess_GenSmoothNormals |					// generate smooth normal vectors if not existing
			aiProcess_SplitLargeMeshes |					// split large, unrenderable meshes into submeshes
			aiProcess_Triangulate |							// triangulate polygons with more than 3 edges
			aiProcess_SortByPType |							// make 'clean' meshes which consist of a single typ of primitives
			aiProcess_GenSmoothNormals |					// if no normals then create them
			0;

		// Create an instance of the Importer class
		Assimp::Importer importer;

		// By removing all points and lines we guarantee a face will describe a 3 vertex triangle
		importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_LINE | aiPrimitiveType_POINT);
		importer.SetPropertyInteger(AI_CONFIG_GLOB_MEASURE_TIME, 1);

		const aiScene* scene = importer.ReadFile(objFilename.c_str(), ppsteps);

		

		if (!scene)
		{
			EsOutput(importer.GetErrorString());
			return false;
		}

		return PopulateFromAssimpScene(scene);
	}

	// Parse the ASSIMP data into our format
	bool ModelLoader::PopulateFromAssimpScene(const aiScene* scene)
	{
		// An assimp scene can contain many things I do not need like cameras and lights
		// Some I may want to support in the future so output that these exist but are being ignored:
		if (scene->HasCameras())
			EsOutput("Ignoring: Scene has camera");
		if (scene->HasLights())
			EsOutput("Ignoring: Scene has lights");

		if (!scene->HasMeshes())
		{
			EsOutput("Scene has no mesh");
			return false;
		}

		// Materials are held scene wide and referenced in the part by id so need to grab here
		m_materials.resize(scene->mNumMaterials);
		for (unsigned int m = 0; m < scene->mNumMaterials; m++)
		{
			// Materials are held against property keys
			aiColor4D col;
			if (AI_SUCCESS == scene->mMaterials[m]->Get(AI_MATKEY_COLOR_AMBIENT, col))
				m_materials[m].ambientColour = aiColor4DToGlmVec4(col);

			if (AI_SUCCESS == scene->mMaterials[m]->Get(AI_MATKEY_COLOR_DIFFUSE, col))
				m_materials[m].diffuseColour = aiColor4DToGlmVec4(col);

			if (AI_SUCCESS == scene->mMaterials[m]->Get(AI_MATKEY_COLOR_SPECULAR, col))
				m_materials[m].specularColour = aiColor4DToGlmVec4(col);

			if (AI_SUCCESS == scene->mMaterials[m]->Get(AI_MATKEY_COLOR_EMISSIVE, col))
				m_materials[m].emissiveColour = aiColor4DToGlmVec4(col);

			unsigned int shininess = 0;
			unsigned int ret1 = scene->mMaterials[m]->Get(AI_MATKEY_SHININESS, shininess);
			unsigned int shininessStrength = 0;
			unsigned int ret2 = scene->mMaterials[m]->Get(AI_MATKEY_SHININESS_STRENGTH, shininessStrength);
			float specularFactor = 0;
			if (ret1 == AI_SUCCESS && ret2 == AI_SUCCESS)
				m_materials[m].specularFactor = (float)(shininess * shininessStrength);
			else if (ret1 == AI_SUCCESS)
				m_materials[m].specularFactor = (float)shininess; // TODO: not sure about this		

			// There are many types for each colour and also normals
			aiString texPath;
			if (AI_SUCCESS == scene->mMaterials[m]->GetTexture(aiTextureType_DIFFUSE, 0, &texPath))
			{
				// Just storing the filename for now, the factory will have to create a texture id
				m_materials[m].diffuseTextureFilename = std::string(texPath.C_Str());
			}

			if (AI_SUCCESS == scene->mMaterials[m]->GetTexture(aiTextureType_SPECULAR, 0, &texPath))
			{
				// Just storing the filename for now, the factory will have to create a texture id
				m_materials[m].specularTextureFilename = std::string(texPath.C_Str());
			}

			// Ignoring others - report
			for (int i = aiTextureType_AMBIENT; i < aiTextureType_UNKNOWN; i++)
			{
				if (AI_SUCCESS == scene->mMaterials[m]->GetTexture((aiTextureType)i, 0, &texPath))
					EsOutput("Ignoring: material texture type: " + std::to_string(i));
			}
		}

		int hasBones{ 0 };
		int hasTangents{ 0 };
		int hasColourChannels{ 0 };
		int hasMMoreThanOneUVChannel{ 0 };

		//EsOutput("Scene contains " + std::to_string(scene->mNumMeshes) + " mesh");

		// ASSIMP mesh
		// http://assimp.sourceforge.net/lib_html/structai_mesh.html
		for (unsigned int i = 0; i < scene->mNumMeshes; i++)
		{
			aiMesh* aimesh = scene->mMeshes[i];

			if (aimesh->HasBones())
				hasBones++;
			if (aimesh->GetNumColorChannels())
				hasColourChannels++;
			if (aimesh->GetNumUVChannels() > 1)
				hasMMoreThanOneUVChannel++;
			if (aimesh->HasTangentsAndBitangents())
				hasTangents++;

			// Create my mesh part
			m_meshVector.push_back(Mesh());
			Mesh& newMesh = m_meshVector.back();

			newMesh.name = aimesh->mName.C_Str();

			//		EsOutput("Processing mesh with name: " + newMesh->name);

					// Copy over all the vertices, ai format of a vertex is same as mine
					// Probably be able to speed this all up at some point
			for (size_t v = 0; v < aimesh->mNumVertices; v++)
			{
				glm::vec3 newV = *(glm::vec3*) & aimesh->mVertices[v];
				newMesh.vertices.push_back(newV);
			}

			// And the normals if there are any
			if (aimesh->HasNormals())
			{
				for (size_t v = 0; v < aimesh->mNumVertices; v++)
				{
					glm::vec3 newN = *(glm::vec3*) & aimesh->mNormals[v];
					newMesh.normals.push_back(newN);
				}
			}

			// And texture coordinates
			if (aimesh->HasTextureCoords(0))
			{
				for (size_t v = 0; v < aimesh->mNumVertices; v++)
				{
					glm::vec2 newN = *(glm::vec2*) & aimesh->mTextureCoords[0][v];
					newMesh.uvCoords.push_back(newN);
				}
			}

			// Faces contain the vertex indices and due to the flags I set before are always triangles
			for (unsigned int face = 0; face < aimesh->mNumFaces; face++)
			{
				EsAssert(aimesh->mFaces[face].mNumIndices == 3);
				for (int triInd = 0; triInd < 3; triInd++)
					newMesh.elements.push_back(aimesh->mFaces[face].mIndices[triInd]);
			}

			// Material index
			newMesh.materialIndex = aimesh->mMaterialIndex;
		}

		if (hasBones)
			EsOutput("Ignoring: One or more mesh have bones");
		if (hasColourChannels)
			EsOutput("Ignoring: One or more mesh has colour channels");
		if (hasMMoreThanOneUVChannel)
			EsOutput("Ignoring: One or more mesh has more than one UV channel");
		if (hasTangents)
			EsOutput("Ignoring: One or more mesh has tangents");

		// Hierarchy, ASSIMP calls these nodes
		m_rootNode = RecurseCreateNode(scene->mRootNode, nullptr);

		for (size_t i = 0; i < scene->mNumAnimations; i++)
		{
			// Only supporting node animation			
			if (scene->mAnimations[i]->mNumMeshChannels)
				EsOutput("Ignoring: mesh animations");

			if (scene->mAnimations[i]->mNumChannels)
				EsOutput("Animation has " + std::to_string(scene->mAnimations[i]->mNumChannels) + " Channels");
		}

		EsOutput("Loaded OK");

		return true;
	}

	// Recursive node creation
	Node* ModelLoader::RecurseCreateNode(aiNode* node, Node* parent)
	{
		Node* newNode = new Node;

		newNode->name = node->mName.C_Str();
		newNode->parentNode = parent;

		for (size_t i = 0; i < node->mNumMeshes; i++)
			newNode->meshIndices.push_back(node->mMeshes[i]);

		newNode->transform = *((glm::mat4*) & node->mTransformation);

		if (node->mMetaData)
			EsOutput("Ignoring: node has metadata");

		for (size_t i = 0; i < node->mNumChildren; i++)
			newNode->childNodes.emplace_back(RecurseCreateNode(node->mChildren[i], newNode));

		return newNode;
	}

	// Recursive deletion of nodes
	void ModelLoader::RecurseDeleteNode(Node* node)
	{
		if (!node)
			return;

		for (Node* n : node->childNodes)
			RecurseDeleteNode(n);

		delete node;
	}

	// Retrieve the dimensions of this model in local coordinates
	void ModelLoader::GetLocalExtents(glm::vec3& minExtents, glm::vec3& maxExtents) const
	{
		if (m_meshVector.empty())
			return;

		m_meshVector[0].GetLocalExtents(minExtents, maxExtents);

		for (size_t i = 1; i < m_meshVector.size(); i++)
		{
			glm::vec3 newMin;
			glm::vec3 newMax;
			m_meshVector[0].GetLocalExtents(newMin, newMax);

			minExtents.x = std::min(minExtents.x, newMin.x);
			minExtents.y = std::min(minExtents.y, newMin.y);
			minExtents.z = std::min(minExtents.z, newMin.z);

			maxExtents.x = std::max(maxExtents.x, newMax.x);
			maxExtents.y = std::max(maxExtents.y, newMax.y);
			maxExtents.z = std::max(maxExtents.z, newMax.z);
		}
	}
}