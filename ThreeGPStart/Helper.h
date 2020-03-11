#pragma once

#include "ExternalLibraryHeaders.h"

namespace Helpers
{
	// Uses GLFW to set up a window via GLFW. Also initialises GLEW and OpenGL.
	GLFWwindow* CreateGLFWWindow(int width, int height, const std::string& title);

	// Loads a whole file into a string e.g. for shader use
	std::string stringFromFile(std::string filepath);	

	// Check program linked without error (i.e. no errors in the shaders)
	bool LinkProgramShaders(GLuint shaderProgram);

	// Load and compile a shader of shaderType from file shaderFilename. Returns 0 on error.
	GLuint LoadAndCompileShader(GLenum shaderType, const std::string& shaderFilename);

	// Check for an OpenGL errot and output its type if there was one
	inline bool CheckForGLError()
	{
		GLenum error{ glGetError() };
		switch (error)
		{
		case GL_NO_ERROR:
			return false;
		case GL_INVALID_ENUM:
			std::cout << "GL_INVALID_ENUM." << std::endl;
			break;
		case GL_INVALID_VALUE:
			std::cout << "GL_INVALID_VALUE." << std::endl;
			break;
		case GL_INVALID_OPERATION:
			std::cout << "GL_INVALID_OPERATION." << std::endl;
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			std::cout << "GL_INVALID_FRAMEBUFFER_OPERATION." << std::endl;
			break;
		case GL_OUT_OF_MEMORY:
			std::cout << "GL_OUT_OF_MEMORY." << std::endl;
			break;
		case GL_STACK_UNDERFLOW:
			std::cout << "GL_STACK_UNDERFLOW." << std::endl;
			break;
		case GL_STACK_OVERFLOW:
			std::cout << "GL_STACK_OVERFLOW." << std::endl;
			break;
		default:
			std::cout << "Unknown" << std::endl;
			break;
		}

		return true;
	}

	// Helper to output a glm::vec3
	inline std::string ToString(glm::vec3 v) {
		return "Pos x:" + std::to_string(v.x) +
			" y:" + std::to_string(v.y) +
			" z:" + std::to_string(v.z);
	};

	// Helper to output a glm::vec4 (as a colour)
	inline std::string ToString(glm::vec4 col)
	{
		return "R: " + std::to_string(col.r) +
			"G: " + std::to_string(col.g) +
			"B: " + std::to_string(col.b) +
			"A: " + std::to_string(col.a);
	}
};

