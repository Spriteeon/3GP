#include "ImageLoader.h"

namespace Helpers
{

	// Attempt to load an image form the file and path provided. Returns false on error.
	bool ImageLoader::Load(const std::string& filepath)
	{
		// Determine the format of the image.
		FREE_IMAGE_FORMAT format{ FreeImage_GetFileType(filepath.c_str(), 0) };

		// Check for not found
		if (format == -1)
		{
			std::cout << "Could not find: " << filepath << std::endl;
			return false;
		}

		// Found image, but couldn't determine the file format? Try again...
		if (format == FIF_UNKNOWN)
		{
			std::cout << "Couldn't determine file format - attempting to get from file extension..." << std::endl;

			format = FreeImage_GetFIFFromFilename(filepath.c_str());

			// Check format is supported
			if (!FreeImage_FIFSupportsReading(format))
			{
				std::cout << "Detected image format cannot be read!" << std::endl;
				return false;
			}
		}

		// If we're here we have a known image format, so load the image into a bitmap
		FIBITMAP* bitmap{ FreeImage_Load(format, filepath.c_str()) };

		// How many bits-per-pixel is the source image?
		unsigned int bitsPerPixel{ FreeImage_GetBPP(bitmap) };

		// Convert our image to 32 bits (8 bits per channel, Red/Green/Blue/Alpha) if not already
		FIBITMAP* bitmap32{ nullptr };
		if (bitsPerPixel == 32)
			bitmap32 = bitmap;
		else
			bitmap32 = FreeImage_ConvertTo32Bits(bitmap);

		// Grab size
		m_width = FreeImage_GetWidth(bitmap32);
		m_height = FreeImage_GetHeight(bitmap32);

		// Get a pointer to the texture data as an array of unsigned bytes.
		// Note: At this point bitmap32 ALWAYS holds a 32-bit colour version of our image - so we get our data from that.
		// Also, we don't need to delete or delete[] this textureData because it's not on the heap (so attempting to do
		// so will cause a crash) - just let it go out of scope and the memory will be returned to the stack.
		BYTE* textureData{ FreeImage_GetBits(bitmap32) };

		m_data = new GLbyte[(size_t)m_width * (size_t)m_height * 1 * 4];

		// Copy to mine, Note: Freeimage data format is GL_BGRA while I need GL_RGBA
		// Iterate through the pixels, copying the data
		// from 'pixels' to 'bits' except in RGBA format.

		for (int pix = 0; pix < m_width * m_height; pix++)
		{
			m_data[pix * 4 + 0] = textureData[pix * 4 + 2];	//r = 3
			m_data[pix * 4 + 1] = textureData[pix * 4 + 1];	//g = 0
			m_data[pix * 4 + 2] = textureData[pix * 4 + 0];	//b = 1
			m_data[pix * 4 + 3] = textureData[pix * 4 + 3];	//a = 2
		}

		// Note that to avoid the above conversion I could just use GL_BGRA when I create the GL textures but  better to be consistant

		// Unload the 32-bit colour bitmap
		FreeImage_Unload(bitmap32);

		// If we had to do a conversion to 32-bit colour, then unload the original
		// non-32-bit-colour version of the image data too. Otherwise, bitmap32 and
		// bitmap point at the same data, and that data's already been free'd, so
		// don't attempt to free it again! (or we'll crash).
		if (bitsPerPixel != 32)
			FreeImage_Unload(bitmap);

		return true;
	}
}