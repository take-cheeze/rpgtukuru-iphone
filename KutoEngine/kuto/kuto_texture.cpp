/**
 * @file
 * @brief Texture class
 * @author project.kuto
 */

#include "kuto_texture.h"
#include "kuto_utility.h"
#include "kuto_error.h"
#include "kuto_graphics_device.h"
#include "kuto_file.h"
#include "kuto_png_loader.h"


namespace kuto {

Texture::Texture()
: name_(GL_INVALID_VALUE), data_(NULL)
, width_(0), height_(0), orgWidth_(0), orgHeight_(0), format_(GL_RGB)
, created_(false)
{
}

Texture::~Texture()
{
	destroy();
}

void Texture::destroy()
{
	if (created_) {
		if (name_ != GL_INVALID_VALUE) {
			glDeleteTextures(1, &name_);
		}
		if (data_) {
			delete[] data_;
		}
	} else {
		handle_.release();
	}
	name_ = GL_INVALID_VALUE;
	data_ = NULL;
}

bool Texture::loadFromFile(const char* filename, bool useAlphaPalette, int hue)
{
	bool ret = handle_.load(filename, useAlphaPalette, hue);

	name_ = handle_.glTexture();
	data_ = handle_.getData();
	width_ = handle_.getWidth();
	height_ = handle_.getHeight();
	orgWidth_ = handle_.getOrgWidth();
	orgHeight_ = handle_.getOrgHeight();
	format_ = handle_.getFormat();
	return ret;
}

bool Texture::loadFromMemory(char* data, int width, int height, int orgWidth, int orgHeight, GLenum format)
{
	data_ = data;
	width_ = width;
	height_ = height;
	orgWidth_ = orgWidth;
	orgHeight_ = orgHeight;
	format_ = format;

	return createGLTexture();
}

bool Texture::createGLTexture()
{
	created_ = true;
	GraphicsDevice* device = GraphicsDevice::instance();
	glGenTextures(1, &name_);
	kuto_assert( name_ != GL_INVALID_VALUE );
	device->setTexture2D(true, name_);
	glTexImage2D(GL_TEXTURE_2D, 0, format_, width_, height_, 0, format_, GL_UNSIGNED_BYTE, data_);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	return true;
}

void Texture::updateImage()
{
	GraphicsDevice* device = GraphicsDevice::instance();
	device->setTexture2D(true, name_);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width_, height_, format_, GL_UNSIGNED_BYTE, data_);
}

}	// namespace kuto
