#include "game_audio_buffer_pool.h"
#include <rpg2k/Project.hpp>

GameAudioBufferPool::GameAudioBufferPool(rpg2k::model::Project const& p)
: project_(p)
{
}


std::auto_ptr<kuto::AudioDevice::Buffer> GameAudioBufferPool::load(rpg2k::SystemString const& name)
{
	return std::auto_ptr<kuto::AudioDevice::Buffer>( new AudioDevice::Buffer(name) );
}

kuto::AudioDevice::Buffer& GameAudioBufferPool::music(rpg2k::SystemString const& name)
{
	Pool::iterator it = music_.find(name);
	if( it == music_.end() ) {
		return *music_.insert( name, load(
			std::string( project_.gameDir() ).append("/").append("Music").append("/").append(name) ) )->second;
	} else { return *it->second; }
}
kuto::AudioDevice::Buffer& GameAudioBufferPool::sound(rpg2k::SystemString const& name)
{
	Pool::iterator it = sound_.find(name);
	if( it == sound_.end() ) {
		return *sound_.insert( name, load(
			std::string( project_.gameDir() ).append("/").append("Sound").append("/").append(name) ) )->second;
	} else { return *it->second; }
}
