#include <SDL/SDL.h>

#include "Audio2D.hpp"
#include "Debug.hpp"
#include "Main.hpp"


namespace rpg2k
{
	static int const MIX_CHUNK_SIZE = 1024;

	Audio2D::Audio2D(Main& m)
	: owner_(m), currentMusic_(AUDIO_OFF)
	{
	// open SDL_mixer
		bool res = Mix_OpenAudio(
			MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT,
			MIX_DEFAULT_CHANNELS, MIX_CHUNK_SIZE
		) == 0; rpg2k_assert(res);
		Mix_AllocateChannels(MIX_CHANNELS);
	// set extensions
		// sound
		#define PP_enumSoundExt(name) soundExt_.push_back(#name);
		PP_allSoundType(PP_enumSoundExt)
		#undef PP_enumSoundExt
		// music
		#define PP_enumMusicExt(name) musicExt_.push_back(#name);
		PP_allMusicType(PP_enumMusicExt)
		#undef PP_enumMusicExt
	}
	Audio2D::~Audio2D()
	{
		for(
			std::map< RPG2kString, Mix_Music* >::const_iterator it = musicPool_.begin();
			it != musicPool_.end(); ++it
		) Mix_FreeMusic(it->second);
		musicPool_.clear();
		for(
			std::map< RPG2kString, Mix_Chunk* >::iterator it = soundPool_.begin();
			it != soundPool_.end(); ++it
		) Mix_FreeChunk(it->second);
		soundPool_.clear();

		Mix_HaltMusic();
		Mix_HaltChannel(-1);
		Mix_CloseAudio();
	}

	Mix_Chunk* Audio2D::loadSound(RPG2kString const& name)
	{
		for(int i = 0; i < Material::RES_END; i++) {
			for(int j = 0; j < SND_END; j++) {
				SystemString fileName =
					owner_.getMaterial().getFileName((Material::Resource)i, Material::Sound, name, soundExt_[j]);
			// Make SDL_RWops
				SDL_RWops* rwop = SDL_RWFromFile(fileName.c_str(), "rb");
				if(rwop == NULL) continue;
			// Load Mix_Chunk
				Mix_Chunk* chunk = Mix_LoadWAV_RW(rwop, 1);
				if(chunk == NULL) continue;
			// Dispose and Exit
				// SDL_FreeRW(rwop);
				cout << "Load Sound success. NAME = " << fileName << ";" << endl;
				return chunk;
			}
		}
		throw std::runtime_error("Cannot load Sound. NAME = " + name.toSystem() + ";");
	}
	Mix_Music* Audio2D::loadMusic(RPG2kString const& name)
	{
		for(int i = 0; i < Material::RES_END; i++) {
			for(int j = 0; j < MUS_END; j++) {
				SystemString fileName =
					owner_.getMaterial().getFileName((Material::Resource)i, Material::Music, name, musicExt_[j]);
			// Load Mix_Music
				Mix_Music* music = Mix_LoadMUS( fileName.c_str() );
				if(music == NULL) continue;
			// Dispose and Exit
				cout << "Load Music success. NAME = " << fileName << ";" << endl;
				return music;
			}
		}
		throw std::runtime_error("Cannot load Music. NAME = " + name.toSystem() + ";");
	}

	Mix_Music* Audio2D::getMusic(RPG2kString const& name)
	{
		if( musicPool_.find(name) == musicPool_.end() )
			musicPool_.insert( std::make_pair( name, loadMusic(name) ) );
		return musicPool_[name];
	}
	Mix_Chunk* Audio2D::getSound(RPG2kString const& name)
	{
		if( soundPool_.find(name) == soundPool_.end() )
			soundPool_.insert( std::make_pair( name, loadSound(name) ) );
		else Mix_HaltChannel(-1);

		return soundPool_[name];
	}

	void Audio2D::stop()
	{
		stopMusic();
		stopSound();
	}
	void Audio2D::stopMusic()
	{
		Mix_HaltMusic();
		currentMusic_ = AUDIO_OFF;
	}
	void Audio2D::stopSound()
	{
		Mix_HaltChannel(-1);
	}

	void Audio2D::playSound(structure::Sound const& se)
	{
		if( se.fileName() != AUDIO_OFF ) {
			Mix_Chunk* sound = getSound(se[1]);
			int channel = Mix_PlayChannel(-1, sound, 0);

			if(channel == -1) {
				cerr << "Mix_PlayChannel Error: " << Mix_GetError() << endl;
				Mix_HaltChannel(0);
				channel = Mix_PlayChannel(0, sound, 0);
			}

			double volMax = MIX_MAX_VOLUME;
			double right =
				(volMax / VOLUME_MAX) *
				( se.volume() + (se.balance() - BALANCE_CENTER) );

			bool res = Mix_SetPanning(channel, volMax*2-1 - right, right) != 0; rpg2k_assert(res);
		}
	}
	void Audio2D::playMusic(structure::Music const& bgm)
	{
		if( bgm.fileName() == AUDIO_OFF ) stopMusic();
		else if( bgm.fileName() != currentMusic_ ) {
			Mix_Music* music = getMusic( bgm.fileName() );
			double volMax = MIX_MAX_VOLUME;

			bool res = Mix_FadeInMusic( music, -1, bgm.fadeInTime() ) == 0; rpg2k_assert(res);

			Mix_VolumeMusic(volMax/VOLUME_MAX * bgm.volume() );

			currentMusic_ = bgm.fileName();
		}
	}

	void Audio2D::gotoTitle()
	{
		stop();
	}
} // namespace rpg2k
