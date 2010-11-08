/**
 * @file
 * @brief Load Texture Handle
 * @author project.kuto
 */
#pragma once

#include "kuto_gl.h"
#include "kuto_load_handle.h"


namespace kuto {

class LoadTextureHandle : public LoadHandle
{
public:
	bool load(const std::string& filename, bool useAlphaPalette, int hue);

	GLuint glTexture() const;
	char* data();
	int width() const;
	int height() const;
	int orgWidth() const;
	int orgHeight() const;
	GLenum format() const;

private:
	virtual LoadCore* createCore(const std::string& filename, const char* subname);
};

}	// namespace kuto
