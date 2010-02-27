/**
 * @file
 * @brief Load Texture Handle
 * @author project.kuto
 */

#include "kuto_load_texture.h"
#include "kuto_load_texture_core.h"
#include "kuto_stringstream.h"


namespace kuto {

bool LoadTextureHandle::load(const std::string& filename, bool useAlphaPalette, int hue)
{
	std::ostringstream ss(useAlphaPalette? "$ay":"$an");
	ss << "$h" << hue;
	return LoadHandle::load(filename, ss.str().c_str());
}

LoadCore* LoadTextureHandle::createCore(const std::string& filename, const char* subname)
{
	return new LoadTextureCore(filename, subname);
}

GLuint LoadTextureHandle::glTexture() const
{
	return core_? static_cast<LoadTextureCore*>(core_)->glTexture() : 0;
}

char* LoadTextureHandle::getData()
{
	return core_? static_cast<LoadTextureCore*>(core_)->getData() : NULL;
}

int LoadTextureHandle::getWidth() const
{
	return core_? static_cast<LoadTextureCore*>(core_)->getWidth() : 0;
}

int LoadTextureHandle::getHeight() const
{
	return core_? static_cast<LoadTextureCore*>(core_)->getHeight() : 0;
}

int LoadTextureHandle::getOrgWidth() const
{
	return core_? static_cast<LoadTextureCore*>(core_)->getOrgWidth() : 0;
}

int LoadTextureHandle::getOrgHeight() const
{
	return core_? static_cast<LoadTextureCore*>(core_)->getOrgHeight() : 0;
}

GLenum LoadTextureHandle::getFormat() const
{
	return core_? static_cast<LoadTextureCore*>(core_)->getFormat() : 0;
}

}	// namespace kuto

