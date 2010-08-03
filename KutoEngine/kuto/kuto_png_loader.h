/**
 * @file
 * @brief PNG Image Loader
 * @author project.kuto
 */
#pragma once

#include "kuto_math.h"
#include <png.h>

namespace kuto {

class LoadTextureCore;


class PngLoader
{
public:
	PngLoader();
	~PngLoader();

	bool createTexture(char* bytes, LoadTextureCore& core, bool useAlphaPalette, int hue);

private:
	char*				bytes_;
	png_structp			pngStruct_;
	png_infop			pngInfo_;
};	// class PngLoader

}	// namespace kuto
