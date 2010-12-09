#include "kuto_audio_device.h"
#include "kuto_error.h"
#include "kuto_file.h"

#include <cstring>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>

#include <boost/array.hpp>

#include <AL/alut.h>


namespace kuto
{
	namespace
	{
		struct SampleData
		{
			ALenum format;
			ALfloat freq;
			std::vector<uint8_t> data;

			SampleData(ALenum fmt, ALfloat frq, std::size_t s)
			: format(fmt), freq(frq), data(s)
			{
			}
		};

		struct Loader
		{
			virtual std::auto_ptr<SampleData> operator()(std::istream& is) const
			{
				return std::auto_ptr<SampleData>();
			}
		}; // struct Loader
		class LoaderWAV : public Loader
		{
			virtual std::auto_ptr<SampleData> operator()(std::istream& is) const
			{
				// TODO
				return std::auto_ptr<SampleData>();
			}
		}; // class LoaderWAV
		class LoaderMP3 : public Loader
		{
			virtual std::auto_ptr<SampleData> operator()(std::istream& is) const
			{
				// TODO
				return std::auto_ptr<SampleData>();
			}
		}; // class LoaderMP3
		class LoaderMIDI : public Loader
		{
			virtual std::auto_ptr<SampleData> operator()(std::istream& is) const
			{
				// TODO
				return std::auto_ptr<SampleData>();
			}
		}; // class LoaderMIDI

		boost::array<Loader, 3> loaders_ = { { LoaderWAV(), LoaderMP3(), LoaderMIDI(), } };

		std::auto_ptr<SampleData> loadSampleData(std::string const& filename)
		{
			{ // ALUT
				ALenum format; ALsizei size; ALfloat freq;
				ALvoid* data = alutLoadMemoryFromFile(filename.c_str(), &format, &size, &freq);
				if( data ) {
					std::auto_ptr<SampleData> ret( new SampleData(format, freq, size) );
					std::memcpy( &(ret->data[0]), data, size );
					std::free(data);
					return ret;
				}
			}

			std::string ext = File::extension(filename);
			std::ifstream ifs( filename.c_str(), std::ios_base::in | std::ios_base::binary );

			for(unsigned i = 0; i < loaders_.size(); i++) {
				std::auto_ptr<SampleData> ret = loaders_[i](ifs);
				if( ret.get() ) return ret;
			}
			return std::auto_ptr<SampleData>();
		}
		std::auto_ptr<SampleData> loadSampleData(uint8_t const* data, size_t size)
		{
			std::istringstream iss( std::string(reinterpret_cast<char const*>(data), size)
			, std::ios_base::in | std::ios_base::binary );

			for(unsigned i = 0; i < loaders_.size(); i++) {
				std::auto_ptr<SampleData> ret = loaders_[i](iss);
				if( ret.get() ) return ret;
			}
			return std::auto_ptr<SampleData>();
		}
	} // anonymous namespace

	AudioDevice::Buffer::Buffer(std::string const& filename)
	: name_(AL_INVALID_VALUE)
	{
		if( !loadFromFile(filename) ) kuto_assert(false);
	}
	AudioDevice::Buffer::Buffer(uint8_t const* data, std::size_t size)
	: name_(AL_INVALID_VALUE)
	{
		if( !loadFromMemory(data, size) ) kuto_assert(false);
	}
	AudioDevice::Buffer::Buffer()
	: name_(AL_INVALID_VALUE)
	{
	}
	AudioDevice::Buffer::~Buffer()
	{
		if( isValid() ) alDeleteBuffers(1, &name_);
	}

	bool AudioDevice::Buffer::loadFromFile(std::string const& filename)
	{
		std::auto_ptr<SampleData> loaded = loadSampleData(filename);
		if( loaded.get() == NULL ) return false;
		this->format_ = loaded->format;
		this->freq_ = loaded->freq;
		this->data_ = loaded->data;

		if( !isValid() ) alGenBuffers(1, &name_);
		this->update();

		return true;
	}
	bool AudioDevice::Buffer::loadFromMemory(uint8_t const* data, std::size_t size)
	{
		std::auto_ptr<SampleData> loaded = loadSampleData(data, size);
		if( loaded.get() == NULL ) return false;
		this->format_ = loaded->format;
		this->freq_ = loaded->freq;
		this->data_ = loaded->data;

		if( !isValid() ) alGenBuffers(1, &name_);
		this->update();

		return true;
	}
	void AudioDevice::Buffer::update()
	{
		alBufferData(name_, this->format_, &(this->data_[0]), this->data_.size(), this->freq_);
	}

	bool AudioDevice::Buffer::isValid() const
	{
		return (name_ != AL_INVALID_VALUE) && alIsBuffer(name_);
	}

	AudioDevice::Source::Source()
	{
		alGenSources(1, &name_); kuto_assert(name_ != AL_INVALID_VALUE);
	}
	AudioDevice::Source::~Source()
	{
		alDeleteSources(1, &name_);
	}
	void AudioDevice::Source::play  () { alSourcePlay  (name_); }
	void AudioDevice::Source::pause () { alSourcePause (name_); }
	void AudioDevice::Source::stop  () { alSourceStop  (name_); }
	void AudioDevice::Source::rewind() { alSourceRewind(name_); }

	AudioDevice::AudioDevice()
	{
		/* // getting supported device
		ALCchar const* str = alcGetString(NULL, ALC_ENUMERATION_EXT);
		kuto_assert(str);
		while(*str) {
			std::string devName;
			while(*str) {
				devName.push_back(*(str++));
			}
			devNames_.push_back(devName);

			std::cout << devName << std::endl;
		}
		 */
		defaultDev_ = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);

		device_ = alcOpenDevice( defaultDev_.c_str() ); kuto_assert(device_);
		context_ = alcCreateContext(device_, NULL); kuto_assert(context_);
		if( alcMakeContextCurrent(context_) != AL_TRUE ) kuto_assert(false);
		alcProcessContext(context_);

		// clear and check error
		if( alGetError() != AL_NO_ERROR) kuto_assert(false);
	}
	AudioDevice::~AudioDevice()
	{
		alcSuspendContext(context_);
		alcDestroyContext(context_);
		if( alcCloseDevice(device_) != AL_TRUE ) kuto_assert(false);
	}

	AudioDevice::Source& AudioDevice::addSource()
	{
		sourceList_.push_back( boost::shared_ptr<Source>( new Source() ) );
		return *sourceList_.back();
	}

	void AudioDevice::playAll()
	{
		alSourcePlayv( sourceList_.size(), &(allSourceName()[0]) );
	}
	void AudioDevice::pauseAll()
	{
		alSourcePausev( sourceList_.size(), &(allSourceName()[0]) );
	}
	void AudioDevice::stopAll()
	{
		alSourceStopv( sourceList_.size(), &(allSourceName()[0]) );
	}
	void AudioDevice::rewindAll()
	{
		alSourceRewindv( sourceList_.size(), &(allSourceName()[0]) );
	}
} // namespace kuto
