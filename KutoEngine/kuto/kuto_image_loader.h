/**
 * @file
 * @brief Image Loader
 * @author project.kuto
 */
#pragma once

#include "kuto_math.h"


namespace kuto {

class LoadTextureCore;


class ImageLoader
{
public:
	ImageLoader();
	~ImageLoader();

	bool createTexture(const char* filename, LoadTextureCore& core);

private:
};	// class PngLoader

}	// namespace kuto
