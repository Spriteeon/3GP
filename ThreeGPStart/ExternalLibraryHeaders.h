#pragma once

// Glew is a library that handles OpenGL extensions for us
#include <glew.h>

// GLFW is a library to handle OpenGL window setup as well as input
#include <GLFW/glfw3.h>

// GLM is a maths library
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// FreeImage is used to load image files
#include <FreeImage.h>

// Assimp is a 3D model loader library
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// Common C++ files used all over
#include <string>
#include <iostream>
#include <vector>
#include <map>