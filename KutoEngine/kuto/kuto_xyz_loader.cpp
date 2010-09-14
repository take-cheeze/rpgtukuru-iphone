/**
 * @file
 * @brief PNG Image Loader
 * @author project.kuto
 */

#include "kuto_xyz_loader.h"
#include "kuto_load_texture_core.h"
#include "kuto_error.h"
#include "kuto_utility.h"

#include <climits>
#include <cstring>
#include <zlib.h>
#include <boost/smart_ptr.hpp>

namespace kuto {


bool XyzLoader::createTexture(char* bytes, LoadTextureCore& core, bool useAlphaPalette, int hue)
{
	if(!bytes) return false;
// read header
	u8* stream = reinterpret_cast< u8* >(bytes);
	if( std::memcmp(stream, getSign(), SIGN_SIZE) != 0 ) return false;
	stream += SIGN_SIZE;
	uint width  = stream[0] | (stream[1] << CHAR_BIT); stream += sizeof(u16);
	uint height = stream[0] | (stream[1] << CHAR_BIT); stream += sizeof(u16);
/*
	FileHeader header;
	std::memcpy( &header, bytes, sizeof(FileHeader) );
	if(
		std::memcmp(header.signature, this->getSign(), SIGN_SIZE) != 0
	) return false;
	uint width = header.width, height = header.height;
 */
// init zlib
	z_stream z;
	z.zalloc   = Z_NULL;
	z.zfree    = Z_NULL;
	z.opaque   = Z_NULL;
	z.next_in  = Z_NULL;
	if( inflateInit(&z) != Z_OK ) return false; // throw z.msg;
// decode palette and image data
	u8*  inbuff = reinterpret_cast< u8* >( stream ); // + sizeof(FileHeader) );
	boost::scoped_array<u8> outBuff( new u8[PALETTE_SIZE + width * height] );
// init zlib
	int status = Z_OK;
	z.next_out = reinterpret_cast< Bytef* >(outBuff.get());
	z.next_in  = reinterpret_cast< Bytef* >( inbuff);
	z.avail_out = PALETTE_SIZE + width * height;
	z.avail_in  = z.avail_out;
	while(true) {
		status = inflate(&z, Z_NO_FLUSH);
		if( (z.avail_out == 0) && (status == Z_STREAM_END) ) break;
		else if( (status != Z_OK) || (status == Z_STREAM_END) ) return false;
	}
// set palette
	Palette* palette = reinterpret_cast< Palette* >(outBuff.get());
// set parameter for GL
	int texDepth  = useAlphaPalette ? 32 : 24;
	uint texWidth  = fixPowerOfTwo(width );
	uint texHeight = fixPowerOfTwo(height);
	int texBytePerPixel = texDepth / CHAR_BIT;
	// allocate a 0x00 cleared buffer
	u8* imageData = new u8[texWidth * texHeight * texBytePerPixel];
	std::memset(imageData, 0x00, texHeight * texWidth * texBytePerPixel );
// convert to texture format
	u8* src = outBuff.get() + PALETTE_SIZE;
	u8* dst = imageData;
	u8 alpha = useAlphaPalette ? 0x00 : 0xff;

	for (uint y = 0; y < height; y++) {
		uint x = 0;
		for(; x < width; x++) {
			Palette& color = palette[*src];
			dst[0] = color.red;
			dst[1] = color.green;
			dst[2] = color.blue;
			if(*src == 0) dst[3] |= alpha;

			src++;
			dst += texBytePerPixel;
		}
		dst += (texWidth - width) * texBytePerPixel;
/*
		for(; x < texWidth; x++) {
			dst[0] = 0;
			dst[1] = 0;
			dst[2] = 0;
			if(useAlphaPalette) dst[3] = 0;
			dst += texBytePerPixel;
		}
 */
	}
/*
	for (uint row = 0; row < texHeight; row++) {
		for (uint i = 0; i < texWidth; i++) {
			if (i >= width || row >= height) {
				dst[0] = 0;
				dst[1] = 0;
				dst[2] = 0;
				if(texBytePerPixel == 4) dst[3] = 0;
				dst += texBytePerPixel;
			} else {
				Palette& color = palette[*src];
				dst[0] = color.red;
				dst[1] = color.green;
				dst[2] = color.blue;
				if(*src == 0) dst[3] |= alpha;

				src++;
				dst += texBytePerPixel;
			}
		}
	}
 */
// create texture
	GLenum format = useAlphaPalette ? GL_RGBA : GL_RGB;
	bool ret = core.createTexture(
		reinterpret_cast< char* >(imageData),
		texWidth, texHeight,
		width, height,
		format
	);
	// delete imageData;

	return ret;
}

}	// namespace kuto
