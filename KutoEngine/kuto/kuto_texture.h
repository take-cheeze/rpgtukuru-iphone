/**
 * @file
 * @brief Texture class
 * @author takuto
 */
#pragma once

#include <OpenGLES/ES1/gl.h>
#include "kuto_load_texture.h"


namespace kuto {

class Texture
{
public:
	Texture();
	~Texture();

	bool loadFromFile(const char* filename, bool useAlphaPalette = false, int hue = 0);
	bool loadFromMemory(char* data, int width, int height) {
		return loadFromMemory(data, width, height, width, height, GL_RGBA);
	}
	bool loadFromMemory(char* data, int width, int height, int orgWidth, int orgHeight, GLenum format);
	void destroy();
	
	bool isValid() const { return name_ != 0; }
	GLuint glTexture() const { return name_; }
	int getWidth() const { return width_; }
	int getHeight() const { return height_; }
	int getOrgWidth() const { return orgWidth_; }
	int getOrgHeight() const { return orgHeight_; }
	const char* getData() const { return data_; }
	char* getData() { return data_; }
	GLenum getFormat() const { return format_; }
	void updateImage();

private:
	bool createGLTexture();
	
private:
	LoadTextureHandle	handle_;		///< Load Handle
	GLuint				name_;			///< OpenGL texture name
	char*				data_;			///< texture bitmap data
	int					width_;			///< texture width
	int					height_;		///< texture height
	int					orgWidth_;		///< original texture width
	int					orgHeight_;		///< original texture height
	GLenum				format_;		///< OpenGL texture format
	bool				created_;		///< create flag
};	// class Texture

}	// namespace kuto
