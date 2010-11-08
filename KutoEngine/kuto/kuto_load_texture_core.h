/**
 * @file
 * @brief Load Texture Core
 * @author project.kuto
 */
#pragma once

#include "kuto_gl.h"
#include "kuto_load_binary_core.h"


namespace kuto {

class LoadTextureCore : public LoadBinaryCore
{
public:
	LoadTextureCore(const std::string& filename, const char* subname);
	virtual ~LoadTextureCore();

	bool createTexture(char* data, int width, int height, int orgWidth, int orgHeight, GLenum format);

	GLuint glTexture() const { return name_; }
	char* data() { return data_; }
	int width() const { return width_; }
	int height() const { return height_; }
	int orgWidth() const { return orgWidth_; }
	int orgHeight() const { return orgHeight_; }
	GLenum format() const { return format_; }

private:
	bool useAlphaPalette() const;
	int hue() const;

private:
	GLuint		name_;			///< OpenGL texture name
	char*		data_;			///< texture bitmap data
	int			width_;			///< texture width
	int			height_;		///< texture height
	int			orgWidth_;		///< original texture width
	int			orgHeight_;		///< original texture height
	GLenum		format_;		///< OpenGL texture format
};

}	// namespace kuto
