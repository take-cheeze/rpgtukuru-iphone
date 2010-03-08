/**
 * @file
 * @brief PNG Image Loader
 * @author project.kuto
 */
#pragma once

#include "kuto_math.h"

typedef unsigned int uint;

namespace kuto {

class LoadTextureCore;

class BmpLoader
{
public:
	BmpLoader();
	~BmpLoader();

	bool createTexture(char* bytes, LoadTextureCore& core, bool useAlphaPalette, int hue);
protected:
	virtual const char* getSignature() { return "BM"; }
private:
	static const uint SIGNATURE_SIZE = 2;

#pragma pack(push, 1)
	struct FileHeader
	{
		uint8_t signature[SIGNATURE_SIZE];
		uint32_t fileSize;
		uint16_t reserved1, reserved2;
		uint32_t offset;
	};
	struct CoreHeader
	{
		uint32_t headerSize;
		uint16_t width, height;
		uint16_t planeNum;
		uint16_t bitsPerPixel;
	};
	struct InfoHeader
	{
		uint32_t headerSize;
		uint32_t width, height;
		uint16_t planeNum;
		uint16_t bitsPerPixel;
		uint32_t compressType;
		uint32_t imageDataSize;
		uint32_t horizontalRes, verticalRes;
		uint32_t usingColorNum;
		uint32_t importantColorNum;
	};
	struct RgbTriple
	{
		uint8_t blue;
		uint8_t green;
		uint8_t red;
	};
	struct RgbQuad
	{
		uint8_t blue;
		uint8_t green;
		uint8_t red;
		uint8_t reserved;
	};
#pragma pack(pop)

	enum CompressType
	{
		BI_RGB = 0,
		BI_RLE8,
		BI_RLE4,
		BI_BITFIELDS,
		BI_JPEG,
		BI_PNG,
	};
};	// class BmpLoader

}	// namespace kuto
