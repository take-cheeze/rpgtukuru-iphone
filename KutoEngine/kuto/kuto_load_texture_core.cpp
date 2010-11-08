/**
 * @file
 * @brief Load Binaly Core
 * @author project.kuto
 */

#include "kuto_load_texture_core.h"
#include "kuto_graphics_device.h"
#include "kuto_file.h"
#include "kuto_png_loader.h"
#include "kuto_xyz_loader.h"
#include "kuto_bmp_loader.h"
#if RPG2K_IS_IPHONE
	#include "kuto_image_loader.h"
#endif
#include <cstdlib>


namespace
{
	bool isReadBytes(const std::string& ext)
	{
		return (ext == "png" || ext == "xyz" || ext == "bmp");
	}
} // namespace


namespace kuto {

LoadTextureCore::LoadTextureCore(const std::string& filename, const char* subname)
: LoadBinaryCore(filename, subname, isReadBytes(File::extension(filename)))
, name_(0), data_(NULL), width_(0), height_(0), orgWidth_(0), orgHeight_(0), format_(GL_RGB)
{
	std::string ext = File::extension(filename);
	if (ext == "png") {
		PngLoader().createTexture(bytes(), *this, useAlphaPalette(), hue());
	} else if (ext == "xyz") {
		XyzLoader().createTexture(bytes(), *this, useAlphaPalette(), hue());
	} else if (ext == "bmp") {
		BmpLoader().createTexture(bytes(), *this, useAlphaPalette(), hue());
	} else {
#if RPG2K_IS_IPHONE
		ImageLoader().createTexture(filename_.c_str(), *this);
#else
		kuto_assert(false);
#endif
	}
}

LoadTextureCore::~LoadTextureCore()
{
	if (name_) {
		glDeleteTextures(1, &name_);
		name_ = 0;
	}
	if (data_ && data_ != bytes()) {
		delete[] data_;
		data_ = NULL;
	}
	releaseBytes();
}

bool LoadTextureCore::createTexture(char* data, int width, int height, int orgWidth, int orgHeight, GLenum format)
{
	data_ = data;
	width_ = width;
	height_ = height;
	orgWidth_ = orgWidth;
	orgHeight_ = orgHeight;
	format_ = format;

	glGenTextures(1, &name_);
	GraphicsDevice::instance().setTexture2D(true, name_);
	glTexImage2D(GL_TEXTURE_2D, 0, format_, width_, height_, 0, format_, GL_UNSIGNED_BYTE, data_);
/*
#if RPG2K_IS_WINDOWS
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
#else
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#endif
 */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	return true;
}

bool LoadTextureCore::useAlphaPalette() const
{
	return subname().find("$ay") != std::string::npos;
}

int LoadTextureCore::hue() const
{
	std::string::size_type i = subname().find("$h");
	if (i != std::string::npos)
		return atoi(&subname().c_str()[i+2]);
	return 0;
}


}	// namespace kuto
