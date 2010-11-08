/**
 * @file
 * @brief Load Texture Handle
 * @author project.kuto
 */

#include "kuto_load_texture.h"
#include "kuto_load_texture_core.h"
#include <cstdio>

namespace kuto {

bool LoadTextureHandle::load(const std::string& filename, bool useAlphaPalette, int hue)
{
	std::string subname(useAlphaPalette? "$ay":"$an");
	subname += "$h";
	char huestr[32];
	sprintf(huestr, "%d", hue);
	subname += huestr;
	return LoadHandle::load(filename, subname.c_str());
}

LoadCore* LoadTextureHandle::createCore(const std::string& filename, const char* subname)
{
	return new LoadTextureCore(filename, subname);
}

GLuint LoadTextureHandle::glTexture() const
{
	return core_? static_cast<LoadTextureCore*>(core_)->glTexture() : 0;
}

char* LoadTextureHandle::data()
{
	return core_? static_cast<LoadTextureCore*>(core_)->data() : NULL;
}

int LoadTextureHandle::width() const
{
	return core_? static_cast<LoadTextureCore*>(core_)->width() : 0;
}

int LoadTextureHandle::height() const
{
	return core_? static_cast<LoadTextureCore*>(core_)->height() : 0;
}

int LoadTextureHandle::orgWidth() const
{
	return core_? static_cast<LoadTextureCore*>(core_)->orgWidth() : 0;
}

int LoadTextureHandle::orgHeight() const
{
	return core_? static_cast<LoadTextureCore*>(core_)->orgHeight() : 0;
}

GLenum LoadTextureHandle::format() const
{
	return core_? static_cast<LoadTextureCore*>(core_)->format() : 0;
}

}	// namespace kuto

