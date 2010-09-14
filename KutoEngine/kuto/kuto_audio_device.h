#pragma once

#include "kuto_al.h"
#include "kuto_singleton.h"
#include "kuto_vector3.h"

#include <boost/smart_ptr.hpp>

#include <deque>
#include <string>
#include <vector>

#include <stdint.h>


#if !RPG2K_IS_PSP // TODO

namespace kuto
{
	class AudioDevice : public Singleton<AudioDevice>
	{
		friend class Singleton<AudioDevice>;
	protected:
		AudioDevice();
		~AudioDevice();

	public:
		typedef ALuint Name;

		std::deque<std::string> const& deviceNames() const { return devNames_; }
		std::string const& defaultDevice() const { return defaultDev_; }

		class Buffer
		{
		public:
			Buffer(std::string const& filename);
			Buffer(uint8_t const* data, std::size_t size);
			Buffer();
			~Buffer();

			bool const loadFromFile(std::string const& filename);
			bool const loadFromMemory(uint8_t const* data, std::size_t size);

			bool const isValid() const;

			Name const name() const { return name_; }
			ALenum const format() const { return format_; }
			std::vector<uint8_t> const& data() const { return data_; }
			std::vector<uint8_t>& data() { return data_; }
			ALfloat const frequency() const { return freq_; }

			void update();

		protected:
			Buffer(Buffer const& src);
			Buffer& operator =(Buffer const& src);

		private:
			Name name_;
			ALenum format_;
			std::vector<uint8_t> data_;
			ALfloat freq_;
		}; // class Buffer
		class Source
		{
			friend class AudioDevice;
		public:
			~Source();

			void play();
			void pause();
			void stop();
			void rewind();

			void setBuffer(Buffer const& buf) { alSourcei(name_, AL_BUFFER, buf.name()); }
			void setPosition(Vector3 const& pos) { alSourcefv(name_, AL_POSITION, pos.pointer()); }
			void setVelocity(Vector3 const& vel) { alSourcefv(name_, AL_VELOCITY, vel.pointer()); }
			void setDirection(Vector3 const& dir) { alSourcefv(name_, AL_DIRECTION, dir.pointer()); }
			void setGain(ALfloat const val) { alSourcef(name_, AL_GAIN, val); } // Volume?
			void setSourceRelative(bool const val) { alSourcei(name_, AL_SOURCE_RELATIVE, val? AL_TRUE : AL_FALSE); }
			void setLooping(bool const val) { alSourcei(name_, AL_LOOPING, val? AL_TRUE : AL_FALSE); }
			void setMinGain(ALfloat const val) { alSourcef(name_, AL_MIN_GAIN, val); }
			void setMaxGain(ALfloat const val) { alSourcef(name_, AL_MAX_GAIN, val); }
			void setPitch(ALfloat const val) { alSourcef(name_, AL_PITCH, val); }

			Name const name() const { return name_; }

		protected:
			Source();
			Source(Source const& src);
			Source& operator =(Source const& src);

		private:
			Name name_;
		}; // class Source
		class Listener // only single Listener can exist
		{
			friend class AudioDevice;
		public:
			void setPosition(Vector3 const& pos) { alListenerfv(AL_POSITION, pos.pointer()); }
			void setVelocity(Vector3 const& vel) { alListenerfv(AL_VELOCITY, vel.pointer()); }
			void setOrientation(Vector3 const& ori) { alListenerfv(AL_ORIENTATION, ori.pointer()); }
			void setGain(float const val) { alListenerf(AL_GAIN, val); } // Volume?

		protected:
			Listener() {}
			~Listener() {}
			Listener(Listener const& src);
			Listener& operator =(Listener const& src);

		private:
		}; // class Listener

		Listener& listener() { return theListener_; }
		std::deque< boost::shared_ptr<Source> >& sourceList() { return sourceList_; }
		Source& addSource();

		void playAll();
		void pauseAll();
		void stopAll();
		void rewindAll();

		// bool changeDevice(std::string const& devName);

	private:
		ALCcontext* context_;
		ALCdevice* device_;

		std::deque<std::string> devNames_;
		std::string defaultDev_;

		Listener theListener_;
		typedef std::deque< boost::shared_ptr<Source> > SourceList;
		SourceList sourceList_;

	protected:
		std::vector<Name> allSourceName() const
		{
			std::vector<Name> ret( sourceList_.size() );
			for(std::size_t i = 0; i < sourceList_.size(); i++) ret[i] = sourceList_[i]->name();
			return ret;
		}
		ALCcontext* context() { return context_; }
		ALCdevice* device() { return device_; }
	}; // class AudioDevice
} // namespace kuto

#endif // !RPG2K_IS_PSP
