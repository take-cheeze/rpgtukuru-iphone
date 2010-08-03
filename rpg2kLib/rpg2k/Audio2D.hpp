#ifndef _INC_RPG2K__AUDIO__AUDIO_2D__HPP
#define _INC_RPG2K__AUDIO__AUDIO_2D__HPP

#include <SDL/SDL_mixer.h>

#include <set>
#include <map>

#include "Material.hpp"


namespace rpg2k
{
	class Main;

	namespace structure
	{
		class Music;
		class Sound;
	}

	#define PP_allSoundType(func) func(wav)
	#define PP_allMusicType(func) func(mid) func(wav) func(mp3)
	class Audio2D
	{
	public:
		#define PP_enumSoundType(name) SND_##name,
		enum SoundType { PP_allSoundType(PP_enumSoundType) SND_END, };
		#undef PP_enumSoundType
		#define PP_enumMusicType(name) MUS_##name,
		enum MusicType { PP_allMusicType(PP_enumMusicType) MUS_END, };
		#undef PP_enumMusicType
	private:
		friend class ::rpg2k::Main;

		Main& owner_;

		std::map< RPG2kString, Mix_Music* > musicPool_;
		std::map< RPG2kString, Mix_Chunk* > soundPool_;

		RPG2kString currentMusic_;
		// std::set< RPG2kString > playingSound_;

		std::vector< SystemString > soundExt_;
		std::vector< SystemString > musicExt_;
	protected:
		void gotoTitle();

		Mix_Music* getMusic(RPG2kString const& name);
		Mix_Chunk* getSound(RPG2kString const& name);

		Mix_Chunk* loadSound(RPG2kString const& name);
		Mix_Music* loadMusic(RPG2kString const& name);
	public:
		Audio2D(Main& m);
		~Audio2D();

		void stop();
		void stopMusic();
		void stopSound();

		void playSound(structure::Sound const&  se);
		void playMusic(structure::Music const& bgm);
	}; // class Audio2D
} // namespace rpg2k

#endif
