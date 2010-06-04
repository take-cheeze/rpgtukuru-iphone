#ifndef _INC__RPG2k__DEFINES_HPP
#define _INC__RPG2k__DEFINES_HPP

// #include <SDL.h>

#include <stdint.h>
#include <stdlib.h>

#include <string>

#include <kuto/kuto_define.h>

// #define RPG2000
// #define RPG2003
#define RPG2000_VALUE

#if defined(RPG2K_IS_WINDOWS)
	typedef unsigned int uint;
#endif

namespace rpg2kLib
{
	const uint MEMBER_MAX = 4;

	const uint ID_MIN = 1;
	const uint DATA_BASE_MAX = 5000;

	const uint MAP_UNIT_MAX = 9999, SAVE_DATA_MAX = 15;

	const uint CHIP_REPLACE_MAX = 144;
	enum ChipSetType { CHIP_LOWER = 0, CHIP_UPPER, };
	const uint CHIP_TYPE_NUM = 2;

	const int CURSOR_SPEED = 4;

	const uint ITEM_MIN = 0, ITEM_MAX = 99;

	const uint
		EV_ID_PARTY = 10001,
		EV_ID_BOAT  = 10002, EV_ID_SHIP = 10003, EV_ID_AIRSHIP = 10004,
		EV_ID_THIS  = 10005;
	const int EVENT_STACK_MAX = 1000;
	const int EVENT_STEP_MAX = 10000;

	enum RPG2kKey
	{
		KEY_NONE = 0,
		KEY_ENTER, KEY_CANCEL,
		KEY_SHIFT, KEY_INSTANT_SHOW = KEY_SHIFT,
		KEY_UP, KEY_RIGHT, KEY_DOWN, KEY_LEFT,
	// test play keys
		// CTRL
		KEY_IGNORE_BLOCK,
		// F9
		KEY_DEBUG,
		// F10
		KEY_CLOSE_EVENT,
	// change modes
		// psp select button
		KEY_QUICK_LOAD,
	};

	enum CharSetDir
	{
		CHAR_DIR_UP = 0, CHAR_DIR_RIGHT, CHAR_DIR_DOWN, CHAR_DIR_LEFT,
	};
	enum CharSetPat
	{
		CHAR_PAT_LEFT = 0, CHAR_PAT_MIDDLE, CHAR_PAT_RIGHT,
	};

	enum EventDir
	{
		EV_DIR_DOWN = 2, EV_DIR_LEFT = 4, EV_DIR_RIGHT = 6, EV_DIR_UP = 8,
	};

	inline CharSetDir toCharSetDir(EventDir dir)
	{
		switch(dir) {
			case EV_DIR_DOWN : return CHAR_DIR_DOWN ;
			case EV_DIR_LEFT : return CHAR_DIR_LEFT ;
			case EV_DIR_RIGHT: return CHAR_DIR_RIGHT;
			case EV_DIR_UP   : return CHAR_DIR_UP   ;
			default: return CHAR_DIR_DOWN;
		}
	}
	inline EventDir toEventDir(RPG2kKey key)
	{
		switch(key) {
			case KEY_UP   : return EV_DIR_UP   ;
			case KEY_LEFT : return EV_DIR_LEFT ;
			case KEY_RIGHT: return EV_DIR_RIGHT;
			case KEY_DOWN : return EV_DIR_DOWN ;
			default: return (EventDir)0;
		}
	}

	enum EventMoveType
	{
		EV_FIXED = 0, EV_RANDOM_MOVE, EV_UP_DOWN, EV_LEFT_RIGHT,
		EV_TO_PARTY, EV_FROM_PARTY, EV_MANUAL_MOVE,
	};

	const int EVENT_START_NUM = 5;
	enum EventStart
	{
		EV_KEY_ENTER = 0, EV_PARTY_TOUCH, EV_EVENT_TOUCH,
		EV_AUTO, EV_PARALLEL, EV_CALLED
	};

	const int EVENT_PRIORITY_NUM = 3;
	enum EventPriority
	{
		EV_PR_BELOW = 0, EV_PR_CHAR, EV_PR_ABOVE,
	};

	const int EQUIP_NUM = 5;
	enum EquipType { EQ_WEAPON  = 0, EQ_SIELD, EQ_ARMOR, EQ_HELMET, EQ_OTHER, };

	const int PARAM_NUM = 6, EQ_PARAM_NUM = 4;
	enum ParamType {
		PRM_HP = 0, PRM_MP,
		PRM_ATTACK, PRM_GAURD, PRM_MIND, PRM_SPEED,
	};

	const uint
		R_MASK = 0x00ff0000, G_MASK = 0x0000ff00,
		B_MASK = 0x000000ff, A_MASK = 0xff000000;

	const uint SCREEN_W = 320, SCREEN_H = 240;

	const int SCROLL_W =  16, SCROLL_H =   8;

	const uint FACE_SIZE = 48;
	const uint CHIP_SIZE = 16;
	const uint CHAR_W = 24, CHAR_H = 32;

	const int MIX_CHUNK_SIZE = 1024;
	const int  FADEIN_MIN =  0,  FADEIN_MAX = 10000;
	const int  VOLUME_MIN =  0,  VOLUME_MAX =   100;
	const int   TEMPO_MIN = 50,   TEMPO_MAX =   150;
	const int
		BALANCE_LEFT = 0, BALANCE_CENTER = 50, BALANCE_RIGHT = 100;
	const char* const AUDIO_OFF = "(OFF)";

	const char* const PATH_SEPR = "/";
	const char* const SETTING_Stream = "setting";

	//#if defined(RPG2K_IS_WINDOWS)
	//	const char* const SYS_ENCODE = "Shift_JIS";
	//#else
		const char* const SYS_ENCODE = "UTF-8";
	//#endif
	const char* const TKOOL_ENCODE = "CP932";

	const int PARAM_MAX = 999, PARAM_MIN = 1;

	const uint32_t VAR_DEF_VAL = 0;
	const bool    SWITCH_DEF_VAL = false;

	#if defined(RPG2000) || defined(RPG2000_VALUE)
		const int32_t VAR_MAX =  999999, VAR_MIN = -999999;

		const int EXP_MAX = 999999, EXP_MIN = 0;
		const int EXP_DEF_VAL = 30;

		const int LV_MAX = 50, LV_MIN = 1;

		const int CHAR_HP_MAX = 999, CHAR_HP_MIN = 1;
		const int MP_MIN = 0;

		#if defined(RPG2000)
			const int ENEMY_HP_MAX = 9999;
			const int PICUTURE_NUM = 20;
		#elif defined(RPG2000_VALUE)
			const int ENEMY_HP_MAX = 99999;
			const int PICUTURE_NUM = 50;
		#endif
		const int ENEMY_HP_MIN = 1;
	#elif defined(RPG2003)
		const int32_t VAR_MAX =  9999999, VAR_MIN = -9999999;

		const int EXP_MAX = 9999999, EXP_MIN = 0;
		const int EXP_DEF_VAL = 300;

		const int LV_MAX = 99, LV_MIN = 1;

		const int CHAR_HP_MAX = 9999, CHAR_HP_MIN = 1;
		const int ENEMY_HP_MAX = 99999, ENEMY_HP_MIN = 1;

		const int PICUTURE_NUM = 50;
	#endif

	static const int MONEY_MIN = 0, MONEY_MAX = VAR_MAX;

	extern uint random();
	inline uint random(uint max) { return random()%max; }
	inline int random(int min, int max) { return random(max-min)+min; }
};

#endif
