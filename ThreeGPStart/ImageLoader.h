#pragma once

#include "ExternalLibraryHeaders.h"

namespace Helpers
{

	// Helper utilising FreeImage to load images / textures
	// Loaded format is guaranteed to be 32 bit RGBA layout
	class ImageLoader
	{
	private:
		int m_width{ 0 };
		int m_height{ 0 };
		GLbyte* m_data{ nullptr };
	public:
		// Width in texels of the image
		int Width() const { return m_width; }

		// Height in texels of the image
		int Height() const { return m_height; }

		// Attempt to load an image form the file and path provided. Returns false on error.
		bool Load(const std::string& filepath);

		// Allows access to the raw bytes that make up the image
		GLbyte* GetData() const { return m_data; }
	};

}