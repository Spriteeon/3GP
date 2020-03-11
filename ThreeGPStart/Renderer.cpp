#include "ExternalLibraryHeaders.h"

#include "Renderer.h"
#include "ImageLoader.h"
#include "Model.h"
#include "ModelTerrain.h"
#include "ModelSkyBox.h"

// On exit must clean up any OpenGL resources e.g. the program, the buffers
Renderer::~Renderer()
{
	glDeleteProgram(m_program);	
	glDeleteBuffers(1, &m_VAO);
}

// Load, compile and link the shaders and create a program object to host them
bool Renderer::CreateProgram()
{
	// Create a new program (returns a unqiue id)
	m_program = glCreateProgram();

	// Load and create vertex and fragment shaders
	GLuint vertex_shader{ Helpers::LoadAndCompileShader(GL_VERTEX_SHADER, "Data/Shaders/vertex_shader.glsl") };
	GLuint fragment_shader{ Helpers::LoadAndCompileShader(GL_FRAGMENT_SHADER, "Data/Shaders/fragment_shader.glsl") };
	if (vertex_shader == 0 || fragment_shader == 0)
		return false;

	// Attach the vertex shader to this program (copies it)
	glAttachShader(m_program, vertex_shader);

	// The attibute 0 maps to the input stream "vertex_position" in the vertex shader
	// Not needed if you use (location=0) in the vertex shader itself
	//glBindAttribLocation(m_program, 0, "vertex_position");

	// Attach the fragment shader (copies it)
	glAttachShader(m_program, fragment_shader);

	// Done with the originals of these as we have made copies
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	// Link the shaders, checking for errors
	if (!Helpers::LinkProgramShaders(m_program))
		return false;

	return !Helpers::CheckForGLError();
}

// Load / create geometry into OpenGL buffers	
bool Renderer::InitialiseGeometry()
{
	//// Load and compile shaders into m_program
	if (!CreateProgram())
		return false;

	ModelSkyBox* skyBox = new ModelSkyBox("Data\\Sky\\Clouds\\skybox.x"); //Create Skybox
	if (!skyBox->Initialise()) 
	{
		//ERROR
		std::cout << "Unable to load texture" << std::endl;
		return false;
	}
	myModels.push_back(skyBox); //Add to model vector

	ModelTerrain* terrain = new ModelTerrain(10000, 64); //Create Terrain
	terrain->Texture("Data\\Textures\\grass.jpg");
	if (!terrain->Initialise())
	{
		//ERROR
		std::cout << "Unable to load texture" << std::endl;
		return false;
	}
	myModels.push_back(terrain); //Add to model vector

	float jeepX = 0.0f;
	float jeepZ = 0.0f;
	Model* jeep = new Model("Data\\Models\\Jeep\\jeep.obj", jeepX, (GetHeight(*terrain, jeepX, jeepZ)) + 50, jeepZ, 1.0f); //Create first Jeep
	jeep->Texture("Data\\Models\\Jeep\\jeep_army.jpg");
	if (!jeep->Initialise())
	{
		//ERROR
		std::cout << "Unable to load texture" << std::endl;
		return false;
	}
	myModels.push_back(jeep); //Add to model vector

	float jeepTwoX = 1000.0f;
	float jeepTwoZ = 1000.0f;
	Model* jeepTwo = new Model("Data\\Models\\Jeep\\jeep.obj", jeepTwoX, (GetHeight(*terrain, jeepTwoX, jeepTwoZ)) + 50, jeepTwoZ, 1.0f); //Create second Jeep
	jeepTwo->Texture("Data\\Models\\Jeep\\jeep_rood.jpg");
	if (!jeepTwo->Initialise())
	{
		//ERROR
		std::cout << "Unable to load texture" << std::endl;
		return false;
	}
	myModels.push_back(jeepTwo); //Add to model vector

	return true;
}

float Renderer::GetHeight(Model& model, float posX, float posZ) //Get terrain Height function
{

	return model.GetHeight(posX, posZ);

}

// Render the scene. Passed the delta time since last called.
void Renderer::Render(const Helpers::Camera& camera, float deltaTime)
{		
	// Configure pipeline settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Uncomment to render in wireframe (can be useful when debugging)
	/*glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);*/

	// Clear buffers from previous frame
	glClearColor(0.0f, 0.0f, 0.0f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Compute viewport and projection matrix
	GLint viewportSize[4];
	glGetIntegerv(GL_VIEWPORT, viewportSize);
	const float aspect_ratio = viewportSize[2] / (float)viewportSize[3];
	glm::mat4 projection_xform = glm::perspective(glm::radians(45.0f), aspect_ratio, 0.5f, 20000.0f);

	// Compute camera view matrix and combine with projection matrix for passing to shader
	glm::mat4 view_xform = glm::lookAt(camera.GetPosition(), camera.GetPosition() + camera.GetLookVector(), camera.GetUpVector());
	//glm::mat4 combined_xform = projection_xform * view_xform;

	// Use our program. Doing this enables the shaders we attached previously.
	glUseProgram(m_program);

	for (auto& model : myModels) //Loop through all models in model vector
	{

		model->Render(camera, m_program, projection_xform, view_xform); //Render all model

	}

	Helpers::CheckForGLError();
}
