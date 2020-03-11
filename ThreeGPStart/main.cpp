/* 
	main.cpp : This file contains the 'main' function. Program execution begins and ends there.

	A set of helper classes and start files for the 3D Graphics Programming module
	
	This project uses a number of helper libraries contained in the External folder within the project directory
	The ExternalLibraryHeaders.h loads their headers

	Visual Studio Project Setup
		The paths for the headers are set in project properties / C/C++ / General
		The paths for the libraries are set in project properties / Linker / General
		The static libraries to link to are listed in Linker / Input
		There are also some runtime DLLs required. These are in the External/Bin directory. In order for Viz to find these
		during debugging the Debugging / environment is appended with the correct path
		If you run the exe outside of Viz these dlls need to be in the same folder as the exe but note that the provided
		MakeDistributable.bat batch file automatically copies them into the correct directory for you

	Keith ditchburn 2019
*/

#include "ExternalLibraryHeaders.h"
#include "Helper.h"
#include "Simulation.h"

int main()
{
	// Use the helper function to set up GLFW, GLEW and OpenGL
	GLFWwindow* window{ Helpers::CreateGLFWWindow(1024, 768, "Simple example") };
	if (!window)
		return -1;

	// Create an instance of the simulation class and initialise it
	// If it could not load, exit gracefully
	Simulation simulation;	
	if (!simulation.Initialise())
	{
		glfwTerminate();
		return -1;
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

	// Enter main GLFW loop until the user closes the window
	while (!glfwWindowShouldClose(window))
	{				
		if (!simulation.Update(window))
			break;
		
		// GLFW updating
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Clean up and exit
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}