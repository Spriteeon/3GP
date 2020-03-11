#include "Helper.h"

#include <fstream>
#include <sstream>

namespace Helpers
{
	// Uses GLFW to set up a window via GLFW. Also initialises GLEW and OpenGL.
	GLFWwindow* CreateGLFWWindow(int width, int height, const std::string& title)
	{
		if (!glfwInit())
		{
			std::cout << "Failed to initialise GLFW" << std::endl;
			return nullptr;
		}

		std::cout << "GLFW initialised" << std::endl;

		glfwWindowHint(GLFW_RED_BITS, 8);
		glfwWindowHint(GLFW_GREEN_BITS, 8);
		glfwWindowHint(GLFW_BLUE_BITS, 8);
		glfwWindowHint(GLFW_ALPHA_BITS, 0);
		glfwWindowHint(GLFW_DEPTH_BITS, 24);
		glfwWindowHint(GLFW_STENCIL_BITS, 8);
		glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3 minimum
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 

#ifdef _DEBUG
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

		GLFWwindow* window{ glfwCreateWindow(width, height, title.c_str(), NULL, NULL) };
		if (!window)
		{
			std::cout << "Failed to create window" << std::endl;
			glfwTerminate();
			return nullptr;
		}

		glfwMakeContextCurrent(window);
		glewExperimental = true; // Needed in core profile

		std::cout << "GLFW window initialised" << std::endl;

		GLenum err{ glewInit() };
		if (GLEW_OK != err)
		{
			std::cout << "Failed to initialise GLEW" << std::endl;

			auto errs = glewGetErrorString(err);

			return nullptr;
		}

		std::cout << "GLEW initialised" << std::endl;

		// The framebuffer size needs to be retrieved for glViewport.
		int fbwidth, fbheight;
		glfwGetFramebufferSize(window, &fbwidth, &fbheight);
		glViewport(0, 0, fbwidth, fbheight);

		glfwSwapInterval(-1);

		return window;
	}

	// Loads a whole file into a string e.g. for shaders
	std::string stringFromFile(std::string filepath)
	{
		std::ifstream fp;
		fp.open(filepath, std::ifstream::in);
		if (fp.is_open() == false) {
			return "";
		}
		std::stringstream ss;
		ss << fp.rdbuf();
		return ss.str();
	}

	// Check shader with id compiled without error
	bool DidShaderCompileOK(GLuint id)
	{
		GLint compile_status = 0;
		glGetShaderiv(id, GL_COMPILE_STATUS, &compile_status);
		if (compile_status != GL_TRUE) {
			const int string_length = 1024;
			GLchar log[string_length] = "";
			glGetShaderInfoLog(id, string_length, NULL, log);
			std::cerr << log << std::endl;

			return false;
		}

		return true;
	}

	// Check program linked without error (i.e. no errors in the shaders)
	bool LinkProgramShaders(GLuint shaderProgram)
	{
		// Do a link
		glLinkProgram(shaderProgram);

		GLint link_status = 0;
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &link_status);
		if (link_status != GL_TRUE) {
			const int string_length = 1024;
			GLchar log[string_length] = "";
			glGetProgramInfoLog(shaderProgram, string_length, NULL, log);
			std::cerr << log << std::endl;
			return false;
		}

		return true;
	}

	// Load and compile a shader of shaderType from file shaderFilename
	GLuint LoadAndCompileShader(GLenum shaderType, const std::string& shaderFilename)
	{
		// Create shaders
		GLuint shaderId{ glCreateShader(shaderType) };

		std::string vShaderString = stringFromFile(shaderFilename);
		if (vShaderString.empty())
		{
			std::cout << "Could not load " << shaderFilename << std::endl;
			return 0;
		}

		const char* asChar{ vShaderString.c_str() };

		std::cout << "Compiling Shader" << shaderFilename << std::endl;

		glShaderSource(shaderId, 1, (const GLchar * *)& asChar, NULL);
		glCompileShader(shaderId);

		if (!DidShaderCompileOK(shaderId))
			return 0;

		std::cout << "Compiled OK" << std::endl;

		return shaderId;
	}
}