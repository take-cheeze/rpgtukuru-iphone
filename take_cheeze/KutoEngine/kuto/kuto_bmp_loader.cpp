
#include "kuto_bmp_loader.h"
#include "kuto_load_texture_core.h"
#include "kuto_error.h"
#include "kuto_utility.h"

#include <cstring>
#include <climits>

namespace kuto {

BmpLoader::BmpLoader()
{
}

BmpLoader::~BmpLoader()
{
}

bool BmpLoader::createTexture(char* bytes, LoadTextureCore& core, bool useAlphaPalette, int hue)
{
	if(!bytes) return false;
// read header
	FileHeader header;
	memcpy( &header, bytes, sizeof(FileHeader) );
	if(
		memcmp(header.signature, this->getSignature(), SIGNATURE_SIZE) != 0
	) return false;

	uint width, height, bpp;
	uint8_t* paletteStart;
	CompressType compType;
	uint8_t* offset = reinterpret_cast< uint8_t* >( bytes + sizeof(FileHeader) );
	switch( *( reinterpret_cast< uint32_t* >( offset ) ) ) {
/*
		case 12: {
			CoreHeader core;
			memccpy( &core, offset, sizeof(CoreHeader) );
			width = core.width;
			height = core.height;
			bpp = core.bitsPerPixel;
			compType = BI_RGB;
			paletteStart = offset + core.headerSize;
		} break;
 */
		case 40:
		// case 64:
		case 108:
		case 124: {
			InfoHeader info;
			memcpy( &info, offset, sizeof(InfoHeader) );
			width = info.width;
			height = info.height;
			bpp = info.bitsPerPixel;
			compType = (CompressType) info.compressType;
			paletteStart = offset + info.headerSize;
		} break;
		default: return false;
	}

	GLenum format;
	int texDepth;
	int texWidth  = fixPowerOfTwo(width ), texHeight = fixPowerOfTwo(height);
	int texByteSize;
	uint8_t* imageData;

	switch(compType) {
		case BI_RGB: break;
		default: return false;
	}

	offset = reinterpret_cast< uint8_t* >(bytes + header.offset);
	switch(bpp) {
		case 8: {
		// set palette
			RgbQuad* palette = reinterpret_cast< RgbQuad* >(paletteStart);
		// set parameter for GL
			format = useAlphaPalette ? GL_RGBA : GL_RGB;
			texDepth  = useAlphaPalette ? 32 : 24;
			texByteSize = texDepth / CHAR_BIT;
			imageData = new uint8_t[texWidth * texHeight * texByteSize];
		// convert to texture format
			uint align = (width%4 == 0) ? width : ( (width/4 + 1) * 4 );
			uint8_t alpha = useAlphaPalette ? 0x00 : 0xff;

			for (uint row = 0; row < width; row++) {
				uint8_t* dst = imageData + (width - row - 1)*texWidth;
				uint8_t* src = offset + row*align;
				for (uint i = 0; i < width; i++) {
					RgbQuad& color = palette[*src];
					dst[0] = color.red;
					dst[1] = color.green;
					dst[2] = color.blue;

					if(*src == 0) dst[3] |= alpha;

					src++;
					dst += texByteSize;
				}
			}
		} break;
		case 24: {
			format = GL_RGB;
			texDepth  = bpp;
			texByteSize = texDepth / CHAR_BIT;
			imageData = new uint8_t[texWidth * texHeight * texByteSize];

			uint align = ( (width%4 == 0) ? width : ( (width/4 + 1) * 4 ) ) * texByteSize;

			for (uint row = 0; row < width; row++) {
				memcpy(
					imageData + (width - row - 1)*texWidth,
					offset + row*align,
					width*texByteSize
				);
			}
		} break;
		case 32: {
			format = GL_RGBA;
			texDepth  = bpp;
			texByteSize = texDepth / CHAR_BIT;
			imageData = new uint8_t[texWidth * texHeight * texByteSize];

			uint align = ( (width%4 == 0) ? width : ( (width/4 + 1) * 4 ) ) * texByteSize;

			for (uint row = 0; row < width; row++) {
				memcpy(
					imageData + (width - row - 1)*texWidth,
					offset + row*align,
					width*texByteSize
				);
			}
		} break;
		default: return false;
	}
// create texture
	bool ret = core.createTexture(
		reinterpret_cast< char* >(imageData),
		texWidth, texHeight, width, height, format
	);
	// delete imageData;

	return ret;
}

}	// namespace kuto
