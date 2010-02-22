/**
 * @file
 * @brief PNG Image Loader
 * @author project.kuto
 */
#pragma once

#include "kuto_math.h"

namespace kuto {

class LoadTextureCore;

class BmpLoader
{
public:
	BmpLoader();
	~BmpLoader();
	
	bool createTexture(char* bytes, LoadTextureCore& core, bool useAlphaPalette, int hue);

private:
};	// class BmpLoader

}	// namespace kuto
