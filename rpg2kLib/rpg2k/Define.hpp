#ifndef _INC__RPG2K__DEFINES_HPP
#define _INC__RPG2K__DEFINES_HPP

#include <cstdlib>
#include <stdint.h>

#include <string>
#include <memory>

#include "Vector.hpp"


namespace rpg2k
{
/*
	template< typename T >
	struct UniquePtr { typedef std::auto_ptr< T > Type; }; // someday std::unique_ptr
 */
	typedef unsigned int uint;

	class Binary;

	class SystemString;
	class RPG2kString : public std::string
	{
	public:
		explicit RPG2kString() : std::string() {}
		RPG2kString(std::string const& str) : std::string(str) {}
		RPG2kString(std::string const& str, size_t pos, size_t n = npos) : std::string(str, pos, n) {}
		RPG2kString(char const* s, size_t n) : std::string(s, n) {}
		RPG2kString(char const* s) : std::string(s) {}
		RPG2kString(size_t n, char c) : std::string(n, c) {}
		template< class InputIterator > RPG2kString (InputIterator begin, InputIterator end) : std::string(begin, end) {}

		// ~RPG2kString() { this->std::string::~string(); }

		SystemString toSystem() const;
	}; // class RPG2kString
	class SystemString : public std::string
	{
	public:
		explicit SystemString() : std::string() {}
		SystemString(std::string const& str) : std::string(str) {}
		SystemString(std::string const& str, size_t pos, size_t n = npos) : std::string(str, pos, n) {}
		SystemString(char const* s, size_t n) : std::string(s, n) {}
		SystemString(char const* s) : std::string(s) {}
		SystemString(size_t n, char c) : std::string(n, c) {}
		template< class InputIterator > SystemString (InputIterator begin, InputIterator end) : std::string(begin, end) {}

		// ~SystemString() { this->std::string::~string(); }

		RPG2kString toRPG2k() const;
	}; // class SystemString

	namespace
	{
		uint const FRAME_PER_LOOP = 60;

		uint const MEMBER_MAX = 4;

		uint const ID_MIN = 1;
		uint const DATA_BASE_MAX = 5000;

		uint const MAP_UNIT_MAX = 9999, SAVE_DATA_MAX = 15;

		uint const CHIP_REPLACE_MAX = 144;

		int const CURSOR_SPEED = 4;

		uint const ITEM_MIN = 0, ITEM_MAX = 99;

		uint const
			EV_ID_PARTY = 10001,
			EV_ID_BOAT  = 10002, EV_ID_SHIP = 10003, EV_ID_AIRSHIP = 10004,
			EV_ID_THIS  = 10005;
		int const EV_STACK_MAX = 1000;
		int const EV_STEP_MAX = 10000;
		int const INVALID_PAGE_ID = -1;

		int const INVALID_ID = 0;
		int const CONDITION_NORMAL = 0;

		int const EQ_PARAM_NUM = 4;

		Size2D const SCREEN_SIZE(320, 240);

		Size2D const SCROLL_SIZE(16, 8);

		Size2D const FACE_SIZE(48, 48);
		Size2D const CHIP_SIZE(16, 16);
		Size2D const CHAR_SIZE(24, 32);

		Size2D const MONEY_WINDOW_SIZE(88, 32);

		int const CURSOR_H = 16;

		int const FADEIN_MIN =  0, FADEIN_MAX = 10000;
		int const VOLUME_MIN =  0, VOLUME_MAX =   100;
		int const  TEMPO_MIN = 50,  TEMPO_MAX =   150;
		int const BALANCE_LEFT = 0, BALANCE_CENTER = 50, BALANCE_RIGHT = 100;
		int const FADE_IN_DEF =   0;
		int const VOLUME_DEF  = 100;
		int const TEMPO_DEF   = 100;
		int const BALANCE_DEF =  50;
		char const* const AUDIO_OFF = "(OFF)";

		SystemString const PATH_SEPR = "/";
		SystemString const SETTING_FILE = "setting";

		int const PARAM_MAX = 999, PARAM_MIN = 1;

		uint32_t const VAR_DEF_VAL = 0;
		bool const SWITCH_DEF_VAL = false;

		#if ( defined(RPG2000) || defined(RPG2000_VALUE) )
			int32_t const VAR_MAX =  999999, VAR_MIN = -999999;

			int const EXP_MAX = 999999, EXP_MIN = 0;
			int const EXP_DEF_VAL = 30;

			int const LV_MAX = 50, LV_MIN = 1;

			int const CHAR_HP_MAX = 999, CHAR_HP_MIN = 1;
			int const MP_MIN = 0;

			#if defined(RPG2000)
				int const ENEMY_HP_MAX = 9999;
				int const PICUTURE_NUM = 20;
			#elif defined(RPG2000_VALUE)
				int const ENEMY_HP_MAX = 99999;
				int const PICUTURE_NUM = 50;
			#endif
			int const ENEMY_HP_MIN = 1;
		#elif defined(RPG2003)
			int32_t const VAR_MAX =  9999999, VAR_MIN = -9999999;

			int const EXP_MAX = 9999999, EXP_MIN = 0;
			int const EXP_DEF_VAL = 300;

			int const LV_MAX = 99, LV_MIN = 1;

			int const CHAR_HP_MAX = 9999, CHAR_HP_MIN = 1;
			int const ENEMY_HP_MAX = 99999, ENEMY_HP_MIN = 1;

			int const PICUTURE_NUM = 50;
		#endif

		int const MONEY_MIN = 0, MONEY_MAX = VAR_MAX;
	}

	namespace ChipSet { enum Type { BEGIN, LOWER = BEGIN, UPPER, END, }; }
	namespace Key { enum Type {
		NONE,
		ENTER, CANCEL,
		SHIFT, INSTANT_SHOW = SHIFT,
		UP, RIGHT, DOWN, LEFT,
	// test play keys
		// CTRL
		IGNORE_BLOCK,
		// F9
		TO_DEBUG_MODE,
		// F10
		CLOSE_EVENT,
	// change modes
		// psp select button
		QUICK_LOAD,
	}; }

	namespace CharSet
	{
		namespace Dir { enum Type { BEGIN, UP = BEGIN, RIGHT, DOWN, LEFT, END }; }
		namespace Pat { enum Type { BEGIN, LEFT = BEGIN, MIDDLE, RIGHT, END }; }
	}

	namespace EventDir { enum Type { DOWN = 2, LEFT = 4, RIGHT = 6, UP = 8, }; }
	namespace EventMove { enum Type {
		EV_FIXED, EV_RANDOM_MOVE, EV_UP_DOWN, EV_LEFT_RIGHT,
		EV_TO_PARTY, EV_FROM_PARTY, EV_MANUAL_MOVE,
	}; }

	namespace EventStart { enum Type {
		BEGIN,
		KEY_ENTER = BEGIN, PARTY_TOUCH, EVENT_TOUCH,
		AUTO, PARALLEL, CALLED,
		END,
	}; }

	namespace EventPriority { enum Type {
		BEGIN,
		BELOW = BEGIN, CHAR, ABOVE,
		END,
	}; }

	namespace Equip { enum Type {
		BEGIN,
		WEAPON = BEGIN, SHIELD, ARMOR, HELMET, OTHER,
		END,
	}; }
	namespace Item { enum Type {
		BEGIN,
		NORMAL = BEGIN,
		WEAPON, SHIELD, ARMOR, HELMET, ACCESSORY,
		MEDICINE, BOOK, SEED, SPECIAL, SWITCH,
		END,
	}; }

	namespace Param { enum Type {
		BEGIN,
		HP = BEGIN, MP,
		ATTACK, GAURD, MIND, SPEED,
		END,
	}; }

	namespace Skill{ enum Type {
		BEGIN,
		NORMAL = BEGIN,
		TELEPORT, ESCAPE,
		SWITCH,
		END,
	}; }

	inline CharSet::Dir::Type toCharSetDir(EventDir::Type dir)
	{
		switch(dir) {
			case EventDir::DOWN : return CharSet::Dir::DOWN ;
			case EventDir::LEFT : return CharSet::Dir::LEFT ;
			case EventDir::RIGHT: return CharSet::Dir::RIGHT;
			case EventDir::UP   : return CharSet::Dir::UP   ;
			default: rpg2k_assert(false); return CharSet::Dir::DOWN;
		}
	}
	inline EventDir::Type toEventDir(Key::Type key)
	{
		switch(key) {
			case Key::UP   : return EventDir::UP   ;
			case Key::LEFT : return EventDir::LEFT ;
			case Key::RIGHT: return EventDir::RIGHT;
			case Key::DOWN : return EventDir::DOWN ;
			default: rpg2k_assert(false); return EventDir::DOWN;
		}
	}

	extern uint random();
	inline uint random(uint max) { return random()%max; }
	inline int random(int min, int max) { return random(max-min)+min; }

	template< typename T > bool within(T a, T v, T b) { return ( (a <= v) && (v < b) ); }
	template< typename T > bool within(T v, T b) { return ::rpg2k::within(T(0), v, b); }
	template< typename T > T limit(T min, T v, T max) { return ( (v < min) ? min : (max < v) ? max : v ); }
	template< typename T > T limit(T v, T max) { return ::rpg2k::limit(T(0), v, max); }

	namespace Action
	{
		typedef uint Type;
		namespace Move { enum {
			UP = 0, RIGHT, DOWN, LEFT,
			RIGHT_UP, RIGHT_DOWN, LEFT_DOWN, LEFT_UP,
			RANDOM, TO_PARTY, FROM_PARTY, A_STEP,
		}; }
		namespace Face { enum {
			UP = Move::A_STEP + 1, RIGHT, DOWN, LEFT,
		}; }
		namespace Turn { enum {
			RIGHT_90 = Face::LEFT + 1, LEFT_90, OPPOSITE,
			RIGHT_OR_LEFT_90, RANDOM, TO_PARTY, OPPOSITE_OF_PARTY,
		}; }
		enum {
			HALT = Turn::OPPOSITE_OF_PARTY + 1,
			BEGIN_JUMP, END_JUMP,
			FIX_DIR, UNFIX_DIR,
			SPEED_UP, SPEED_DOWN,
			FREQ_UP, FREQ_DOWN,
			SWITCH_ON, SWITCH_OFF,
			CHANGE_CHAR_SET, PLAY_SOUND,
			BEGIN_SLIP, END_SLIP,
			BEGIN_ANIME, END_ANIME,
			TRANS_UP, TRANS_DOWN, // transparentcy
		};
	}

	/*
	 * hiragana used with vocabulary
	 */
	 namespace hiragana
	 {
		static const char GA[] = { 0x82, 0xaa, 0x00 };
		static const char HA[] = { 0x82, 0xcd, 0x00 };
		static const char NI[] = { 0x82, 0xc9, 0x00 };
		static const char NO[] = { 0x82, 0xcc, 0x00 };
		static const char WO[] = { 0x82, 0xF0, 0x00 };
	}
} // namespace rpg2k

#endif
