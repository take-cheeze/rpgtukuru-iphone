#include <map>
#include <memory>
#include <string>

#include <boost/smart_ptr.hpp>

#include <kuto/kuto_audio_device.h>
#include <kuto/kuto_array.h>

namespace rpg2k { namespace model { class Project; } }

#define PP_audioMaterial(func) \
	func(Music) \
	func(Sound) \


class GameAudioBufferPool
{
public:
	enum Type
	{
		#define PP_enum(name) name,
		PP_audioMaterial(PP_enum)
		#undef PP_enum
		TYPE_END,
	};

public:
	GameAudioBufferPool(rpg2k::model::Project const& p);

	kuto::AudioDevice::Buffer& get(GameAudioBufferPool::Type const t, std::string const& name);

	void clear();

protected:
	std::auto_ptr<kuto::AudioDevice::Buffer> load(GameAudioBufferPool::Type const t, std::string const& name) const;

private:
	typedef std::map< std::string, boost::shared_ptr<kuto::AudioDevice::Buffer> > BufPoolType;

	rpg2k::model::Project const& proj_;
	kuto::Array<BufPoolType, TYPE_END> pool_;

	static char const* DIR_NAME[];
}; // class GameAudioBufferPool
