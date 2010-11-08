#include <kuto/kuto_graphics2d.h>
#include <rpg2k/Project.hpp>
#include "game_texture_pool.h"


char const* GameTexturePool::DIR_NAME[] =
{
	#define PP_enum(name) #name,
	PP_imageMaterial(PP_enum)
	#undef PP_enum
};
bool const GameTexturePool::TRANS[] =
{
	#define PP_enum(name) true,
	PP_transMaterial(PP_enum)
	#undef PP_enum
	#define PP_enum(name) false,
	PP_nonTransMaterial(PP_enum)
	#undef PP_enum
};

GameTexturePool::GameTexturePool(rpg2k::model::Project const& p)
: proj_(p)
{
}
GameTexturePool::~GameTexturePool()
{
}


kuto::Texture& GameTexturePool::get(GameTexturePool::Type t, rpg2k::SystemString const& name)
{
	kuto_assert(t != Picture);
	kuto_assert(t < TYPE_END);

	TexPoolType::iterator it = pool_[t].find(name);
	if( it == pool_[t].end() ) {
		return *pool_[t].insert( name, load(t, name, TRANS[t]) ).first->second;
	} else return *it->second;
}
kuto::Texture& GameTexturePool::picture(rpg2k::SystemString const& name, bool const trans)
{
	TexPoolType::iterator it = picturePool_[trans].find(name);
	if( it == picturePool_[trans].end() ) {
		return *picturePool_[trans].insert( name
		, load(GameTexturePool::Picture, name, trans) ).first->second;
	} else return *it->second;
}

std::auto_ptr< kuto::Texture > GameTexturePool::load(GameTexturePool::Type t, std::string const& name, bool const trans) const
{
	std::auto_ptr<kuto::Texture> newed(new kuto::Texture);
	bool res = RPG2kUtil::LoadImage(*newed,
		std::string( proj_.gameDir() ).append("/").append(DIR_NAME[t]).append("/").append(name), trans);
	kuto_assert(res);
	return newed;
}

void GameTexturePool::clear()
{
	for(kuto::Array<TexPoolType, TYPE_END>::iterator it = pool_.begin(); it < pool_.end(); ++it) {
		it->clear();
	}
	for(kuto::Array<TexPoolType, 2>::iterator it = picturePool_.begin(); it < picturePool_.end(); ++it) {
		it->clear();
	}
}
