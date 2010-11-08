/**
 * @file
 * @brief PNG Image Loader
 * @author project.kuto
 */
#pragma once

#include "kuto_math.h"

namespace kuto {

class LoadTextureCore;


class XyzLoader
{
public:
	bool createTexture(char* bytes, LoadTextureCore& core, bool useAlphaPalette, int hue);

private:
	static const int BIT_DEPTH = 8;
	static const int COLOR_NUM = 256;
	static const int SIGN_SIZE = 4;
#pragma pack(push, 1)
	struct Palette {
		u8 red, green, blue;
	};
#pragma pack(pop)

	static const int PALETTE_SIZE = sizeof(Palette) * COLOR_NUM;
protected:
	virtual const char* signature() { return "XYZ1"; }
};	// class XyzLoader

}	// namespace kuto
