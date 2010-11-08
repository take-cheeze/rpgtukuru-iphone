#include "kuto_bmp_loader.h"
#include "kuto_load_texture_core.h"
#include "kuto_error.h"
#include "kuto_utility.h"

#include <cstring>
#include <climits>
#include <boost/smart_ptr.hpp>


namespace kuto {

bool BmpLoader::createTexture(char* bytes, LoadTextureCore& core, bool useAlphaPalette, int hue)
{
	if(!bytes) return false;
// read header
	FileHeader header;
	memcpy( &header, bytes, sizeof(FileHeader) );
	if(
		memcmp(header.signature, this->signature(), SIGNATURE_SIZE) != 0
	) return false;

	uint width, height, bpp;
	u8* paletteStart;
	CompressType compType;
	u8* offset = reinterpret_cast< u8* >( bytes + sizeof(FileHeader) );
	switch( *( reinterpret_cast< u32* >( offset ) ) ) {
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
	int texWidth  = fixPowerOfTwo(width);
	int texHeight = fixPowerOfTwo(height);
	int texByteSize;
	u8* imageData;

	switch(compType) {
		case BI_RGB: break;
		default: return false;
	}

	offset = reinterpret_cast< u8* >(bytes + header.offset);
	switch(bpp) {
		case 8: {
		// set palette
			RgbQuad* palette = reinterpret_cast< RgbQuad* >(paletteStart);
		// set parameter for GL
			format = useAlphaPalette ? GL_RGBA : GL_RGB;
			texDepth  = useAlphaPalette ? 32 : 24;
			texByteSize = texDepth / CHAR_BIT;
			imageData = new u8[texWidth * texHeight * texByteSize];
			memset(imageData, 0, texWidth * texHeight * texByteSize);
		// convert to texture format
			uint align = (width%LINE_ALIGN == 0) ? width : ( (width/LINE_ALIGN + 1) * LINE_ALIGN );

			for (uint row = 0; row < height; row++) {
				u8* dst = imageData + (height - row - 1) * texWidth * texByteSize;
				u8* src = offset + row * align;
				for (uint i = 0; i < width; i++) {
					RgbQuad const& color = palette[*src];
					dst[0] = color.red;
					dst[1] = color.green;
					dst[2] = color.blue;
					if (useAlphaPalette)
						dst[3] = (*src == 0)? 0x00 : 0xff;

					src++;
					dst += texByteSize;
				}
			}
		} break;
		case 24: {
			format = GL_RGB;
			texDepth  = bpp;
			texByteSize = texDepth / CHAR_BIT;
			imageData = new u8[texWidth * texHeight * texByteSize];

			uint align = ( (width%LINE_ALIGN == 0) ? width : ( (width/LINE_ALIGN + 1) * LINE_ALIGN ) ) * texByteSize;

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
			imageData = new u8[texWidth * texHeight * texByteSize];

			uint align = ( (width%LINE_ALIGN == 0) ? width : ( (width/LINE_ALIGN + 1) * LINE_ALIGN ) ) * texByteSize;

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
