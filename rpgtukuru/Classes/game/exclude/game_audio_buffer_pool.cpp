#include <kuto/kuto_graphics2d.h>
#include <rpg2k/Project.hpp>

#include "game_audio_buffer_pool.h"


namespace
{
	char const* EXTS[] = { ".wav", ".mp3", ".mid", };
} // anonyomous namespace

char const* GameAudioBufferPool::DIR_NAME[] =
{
	#define PP_enum(name) #name,
	PP_audioMaterial(PP_enum)
	#undef PP_enum
};

GameAudioBufferPool::GameAudioBufferPool(rpg2k::model::Project const& p)
: proj_(p)
{
}

kuto::AudioDevice::Buffer& GameAudioBufferPool::get(GameAudioBufferPool::Type const t, std::string const& name)
{
	kuto_assert(t < TYPE_END);
	if( pool_[t].find(name) == pool_[t].end() ) {
		pool_[t].insert( std::make_pair( name, load(t, name).release() ) );
	}
	return *pool_[t].find(name)->second;
}

std::auto_ptr<kuto::AudioDevice::Buffer> GameAudioBufferPool::load(GameAudioBufferPool::Type const t, std::string const& name) const
{
	std::auto_ptr<kuto::AudioDevice::Buffer> newed(new kuto::AudioDevice::Buffer);
	std::string const base = std::string( proj_.gameDir() ).append("/").append(DIR_NAME[t]).append("/").append(name);

	for(unsigned i = 0; i < ( sizeof(EXTS) / sizeof(EXTS[0]) ); i++) {
		bool const res = newed->loadFromFile(base + EXTS[i]);
		if(res) return newed;
	}

	kuto_assert(false);
	return std::auto_ptr<kuto::AudioDevice::Buffer>();
}

void GameAudioBufferPool::clear()
{
	for(kuto::Array<BufPoolType, TYPE_END>::iterator it = pool_.begin(); it < pool_.end(); ++it) it->clear();
}
