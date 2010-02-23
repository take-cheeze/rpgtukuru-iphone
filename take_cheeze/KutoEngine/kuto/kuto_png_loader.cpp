/**
 * @file
 * @brief PNG Image Loader
 * @author project.kuto
 */

#include "kuto_png_loader.h"
#include "kuto_load_texture_core.h"
#include "kuto_error.h"
#include "kuto_utility.h"
#include <cstring>


namespace kuto {

class PngReaderIO
{
public:
	PngReaderIO(char* data) : data_(data), pos_(0) {}
	void read(png_bytep data, png_size_t length) {
		std::memcpy(data, data_ + pos_, length);
		pos_ += length;
	}
private:
	char*			data_;		///< binary data pointer
	int				pos_;		///< current position
};

static void pngReadFunc(png_structp png_ptr, png_bytep data, png_size_t length)
{
	PngReaderIO* reader = (PngReaderIO*)png_get_io_ptr(png_ptr);
	reader->read(data, length);
}



PngLoader::PngLoader()
{
	pngStruct_ = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	pngInfo_ = png_create_info_struct(pngStruct_);
}

PngLoader::~PngLoader()
{
	png_destroy_read_struct(&pngStruct_, &pngInfo_, NULL);
}

bool PngLoader::createTexture(char* bytes, LoadTextureCore& core, bool useAlphaPalette, int hue)
{
	bytes_ = bytes;
	if (!bytes_)
		return false;
	
	PngReaderIO reader(bytes_);
	png_set_read_fn(pngStruct_, (png_voidp)&reader, (png_rw_ptr)pngReadFunc);
	png_read_info(pngStruct_, pngInfo_);

	png_uint_32 pngWidth;
	png_uint_32 pngHeight;
	int pngDepth;
	int pngColorType;
	png_get_IHDR(pngStruct_, pngInfo_, &pngWidth, &pngHeight, &pngDepth, &pngColorType, NULL, NULL, NULL);

	int width = fixPowerOfTwo(pngWidth);		// fix 2^x
	int height = fixPowerOfTwo(pngHeight);		// fix 2^x
	int depth = 24;
	//int colorType;
	//int palSize = 0;
	png_color* palette = NULL;
	int num_palette = 0;
	if (pngColorType == PNG_COLOR_TYPE_RGB) {
		depth = 24;
	} else if (pngColorType == PNG_COLOR_TYPE_RGB_ALPHA) {
		depth = 32;
	} else if (pngColorType == PNG_COLOR_TYPE_PALETTE) {
		//png_bytep trans;
		//int num_trans = 0;
		//if (png_get_tRNS(pngStruct_, pngInfo_, &trans, &num_trans, NULL))
		//	depth = 32;
		//else
		//	depth = 24;
		if (useAlphaPalette)
			depth = 32;
		else
			depth = 24;
		png_get_PLTE(pngStruct_, pngInfo_, &palette, &num_palette);
		if (hue != 0) {
			// hue shift
			for (int i = 0; i < num_palette; i++) {
				png_color color = palette[i];
				ColorHSV hsv = Color(color.red / 255.f, color.green / 255.f, color.blue / 255.f, 1.f).hsv();
				hsv.h += hue;
				if (hsv.h < 0)
					hsv.h += 360;
				else if (hsv.h >= 360)
					hsv.h -= 360;
				Color rgb = hsv.rgb();
				palette[i].red = (u8)(rgb.r * 255.f);
				palette[i].green = (u8)(rgb.g * 255.f);
				palette[i].blue = (u8)(rgb.b * 255.f);
			}
		}
		//png_set_expand(pngStruct_);
		//png_set_filler(pngStruct_, 0, 1);
	} else {
		kuto_assert(0);		// No Support
#if 0
		switch (pngDepth) {
		case 2:  nDepth = 4; break;
		case 16: nDepth = 8; break;
		default: nDepth = nPngDepth; break;
		}
		depth = 32;
		palSize = 1 << nDepth;
#endif
	}
    if (pngDepth == 16)
        png_set_strip_16(pngStruct_);

	//png_set_bgr(pngStruct_);
	png_read_update_info(pngStruct_, pngInfo_);
	
    int pngRowbytes = png_get_rowbytes(pngStruct_, pngInfo_);
    int pngChannels = png_get_channels(pngStruct_, pngInfo_);
	png_bytep pngData = new png_byte[pngRowbytes * pngHeight];
	png_bytep* rows = new png_bytep[height];
	for (int i = 0; i < height; i++) {
		rows[i] = (png_bytep)&pngData[i * pngRowbytes];
	}
	
	png_read_image(pngStruct_, rows);
	png_read_end(pngStruct_, pngInfo_);

	int imageByteSize = (depth / 8);
	int imageRowSize = width * imageByteSize;
	int imageSize = imageRowSize * height;
	char* imageData = new char[imageSize];
    for (uint row = 0; row < pngHeight; row++) {
        png_bytep src = pngData + row * pngRowbytes;
        png_bytep dest = (png_bytep)(imageData + row * imageRowSize);

        if (pngChannels == 1) {     // パレット（またはグレースケール）
            for (uint i = 0; i < pngWidth; i++) {
                png_color color = palette[*src];
                dest[0] = color.red;
                dest[1] = color.green;
                dest[2] = color.blue;
				if (depth == 32)
					dest[3] = *src == 0? 0x00:0xFF;
                src++;
                dest += imageByteSize;
            }
        } else {  // RGB or RGBA
            for (uint i = 0; i < pngWidth; i++) {
                dest[0] = src[0];  // b
                dest[1] = src[1];  // g
                dest[2] = src[2];  // r
				if (depth == 32)
					dest[3] = src[3];
				src += pngChannels;
				dest += imageByteSize;
            }
        }
    }
	delete[] pngData;
	delete[] rows;
	
	GLenum format = (depth == 24)? GL_RGB:GL_RGBA;
	bool ret = core.createTexture(imageData, width, height, pngWidth, pngHeight, format);
	// delete imageData;

	return ret;
}

}	// namespace kuto
