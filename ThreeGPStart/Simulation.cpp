#include "Simulation.h"
#include "Renderer.h"
#include "Model.h"

// Initialise this as well as the renderer, returns false on error
bool Simulation::Initialise()
{
	// Set up camera
	m_camera = std::make_shared<Helpers::Camera>();
	m_camera->Initialise(glm::vec3(0, 2000, 3000), glm::vec3(0.5, 0, 0)); // Jeep
	//m_camera->Initialise(glm::vec3(-13.82f, 5.0f, 1.886f), glm::vec3(0.25f, 1.5f, 0), 30.0f,0.8f); // Aqua pig

	// Set up renderer
	m_renderer = std::make_shared<Renderer>();
	return m_renderer->InitialiseGeometry();
}

// Handle any user input. Return false if program should close.
bool Simulation::HandleInput(GLFWwindow* window)
{

	//Player Movement
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) //Forward
	{
		m_renderer->myModels[2]->Move(10, 0, 0);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) //Left
	{
		m_renderer->myModels[2]->Move(0, 0, -10);
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) //Backwards
	{
		m_renderer->myModels[2]->Move(-10, 0, 0);
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) //Right
	{
		m_renderer->myModels[2]->Move(0, 0, 10);
	}

	// To see an example of input using GLFW see the camera.cpp file.
	return true;
}

// Update the simulation (and render) returns false if program should close
bool Simulation::Update(GLFWwindow* window)
{
	// Deal with any input
	if (!HandleInput(window))
		return false;

	// Calculate delta time since last called
	// We pass the delta time to the camera and renderer
	float timeNow = (float)glfwGetTime();
	float deltaTime{ timeNow - m_lastTime };
	m_lastTime = timeNow;

	// The camera needs updating to handle user input internally
	m_camera->Update(window, deltaTime);

	// Render the scene
	m_renderer->Render(*m_camera, deltaTime);

	return true;
}
