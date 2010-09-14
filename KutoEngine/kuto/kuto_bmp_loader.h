/**
 * @file
 * @brief PNG Image Loader
 * @author project.kuto
 */
#pragma once

#include "kuto_types.h"
#include "kuto_math.h"


namespace kuto {

class LoadTextureCore;

class BmpLoader
{
public:
	bool createTexture(char* bytes, LoadTextureCore& core, bool useAlphaPalette, int hue);

protected:
	virtual const char* getSignature() { return "BM"; }

private:
	static const uint SIGNATURE_SIZE = 2;
	static const uint LINE_ALIGN = 4;

#pragma pack(push, 1)
	struct FileHeader
	{
		u8 signature[SIGNATURE_SIZE];
		u32 fileSize;
		u16 reserved1, reserved2;
		u32 offset;
	};
	struct CoreHeader
	{
		u32 headerSize;
		u16 width, height;
		u16 planeNum;
		u16 bitsPerPixel;
	};
	struct InfoHeader
	{
		u32 headerSize;
		u32 width, height;
		u16 planeNum;
		u16 bitsPerPixel;
		u32 compressType;
		u32 imageDataSize;
		u32 horizontalRes, verticalRes;
		u32 usingColorNum;
		u32 importantColorNum;
	};
	struct RgbTriple
	{
		u8 blue;
		u8 green;
		u8 red;
	};
	struct RgbQuad
	{
		u8 blue;
		u8 green;
		u8 red;
		u8 reserved;
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
