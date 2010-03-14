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

namespace kuto {


XyzLoader::XyzLoader()
{
}

XyzLoader::~XyzLoader()
{
}

bool XyzLoader::createTexture(char* bytes, LoadTextureCore& core, bool useAlphaPalette, int hue)
{
	if(!bytes) return false;
// read header
	FileHeader header;
	memcpy( &header, bytes, sizeof(FileHeader) );
	if(
		memcmp(header.signature, this->getSignature(), SIGNATURE_SIZE) != 0
	) return false;
// init zlib
	z_stream z;
	z.zalloc   = Z_NULL;
	z.zfree    = Z_NULL;
	z.opaque   = Z_NULL;
	z.next_in  = Z_NULL;
	if( inflateInit(&z) != Z_OK ) return false; // throw z.msg;
// decode palette and image data
	uint width = header.width, height = header.height;
	u8*  inbuff = reinterpret_cast< u8* >( bytes + sizeof(FileHeader) );
	u8* outBuff = new u8[PALETTE_SIZE + width * height];
// init zlib
	int status = Z_OK;
	z.next_out = reinterpret_cast< Bytef* >(outBuff);
	z.next_in  = reinterpret_cast< Bytef* >( inbuff);
	z.avail_out = PALETTE_SIZE + width * height;
	z.avail_in  = z.avail_out;
	while(true) {
		status = inflate(&z, Z_NO_FLUSH);
		if( (z.avail_out == 0) && (status == Z_STREAM_END) ) break;
		if( (status != Z_OK) || (status == Z_STREAM_END) ) return false; // throw z.msg;
	}
// set palette
	Palette* palette = reinterpret_cast< Palette* >(outBuff);
// set parameter for GL
	int texDepth  = useAlphaPalette ? 32 : 24;
	int texWidth  = fixPowerOfTwo(header.width );
	int texHeight = fixPowerOfTwo(header.height);
	int texByteSize = texDepth / CHAR_BIT;
	u8* imageData = new u8[texWidth * texHeight * texByteSize];
// convert to texture format
	u8* src = outBuff + PALETTE_SIZE;
	u8* dst = imageData;
	u8 alpha = useAlphaPalette ? 0x00 : 0xff;

	for (uint row = 0; row < texHeight; row++) {
		for (uint i = 0; i < texWidth; i++) {
			if (i >= width || row >= height) {
				dst[0] = 0;
				dst[1] = 0;
				dst[2] = 0;
				if(texByteSize == 4) dst[3] = 0;
				dst += texByteSize;
			} else {
				Palette& color = palette[*src];
				dst[0] = color.red;
				dst[1] = color.green;
				dst[2] = color.blue;
				if(*src == 0) dst[3] |= alpha;
	
				src++;
				dst += texByteSize;
			}
		}
	}
// release
	delete [] outBuff;
// create texture
	GLenum format = useAlphaPalette ? GL_RGBA : GL_RGB;
	bool ret = core.createTexture(
		reinterpret_cast< char* >(imageData),
		texWidth, texHeight,
		header.width, header.height,
		format
	);
	// delete imageData;

	return ret;
}

}	// namespace kuto
