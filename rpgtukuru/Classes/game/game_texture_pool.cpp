#include <memory>
#include <kuto/kuto_graphics2d.h>
#include <rpg2k/Project.hpp>
#include "game_texture_pool.h"


char const* GameTexturePool::DIR_NAME[] =
{
	#define PP_enum(name) #name,
	PP_allMaterial(PP_enum)
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

kuto::Texture& GameTexturePool::get(GameTexturePool::Type t, std::string const& name)
{
	if( pool_[t].find(name) == pool_[t].end() ) {
		pool_[t].insert( std::make_pair( name, load(t, name, TRANS[t]).release() ) );
	}
	return *pool_[t].find(name)->second;
}
kuto::Texture& GameTexturePool::getPicture(std::string const& name, bool const trans)
{
	if( picturePool_[trans].find(name) == picturePool_[trans].end() ) {
		picturePool_[trans].insert( std::make_pair( name, load(GameTexturePool::Picture, name, trans).release() ) );
	}
	return *picturePool_[trans].find(name)->second;
}

std::auto_ptr< kuto::Texture > GameTexturePool::load(GameTexturePool::Type t, std::string const& name, bool const trans) const
{
	std::auto_ptr<kuto::Texture> newed(new kuto::Texture);
	bool res = RPG2kUtil::LoadImage(*newed, std::string( proj_.gameDir() ).append("/").append(DIR_NAME[t]).append("/").append(name), trans); kuto_assert(res);
	return newed;
}

void GameTexturePool::clear()
{
	for(kuto::Array<TexPoolType, TYPE_END>::iterator it = pool_.begin(); it < pool_.end(); ++it) it->clear();
	for(kuto::Array<TexPoolType, 2>::iterator it = picturePool_.begin(); it < picturePool_.end(); ++it) it->clear();
}
