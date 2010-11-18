#pragma once

#include <kuto/kuto_audio_device.h>

#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_unordered_map.hpp>

namespace rpg2k { namespace model { class Project; } }


class GameAudioBufferPool : boost::noncopyable
{
public:
	GameAudioBufferPool(rpg2k::model::Project const& p);

public:
	kuto::AudioDevice::Buffer& music(rpg2k::SystemString const& name);
	kuto::AudioDevice::Buffer& sound(rpg2k::SystemString const& name);

protected:
	std::auto_ptr<kuto::AudioDevice::Buffer> load(rpg2k::SystemString const& name);

private:
	rpg2k::model::Project const& project_;

	typedef boost::ptr_unordered_map<std::string, kuto::AudioDevice::Buffer> Pool;
	Pool music_, sound_;
}; // class GameAudioBufferPool
