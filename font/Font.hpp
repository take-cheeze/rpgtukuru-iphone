#ifndef _INC__RPG2k__FONT_FONT_HPP
#define _INC__RPG2k__FONT_FONT_HPP

#include <string>
#include <stdint.h>

/*
	this is font utility
	all input must be Shift_JIS string
 */

namespace rpg2kLib {
	namespace font {

		enum FontType {
			GOTHIC, MINCHO,
		};
		enum FontColor {
			FNT_NORMAL = 0, FNT_DISABLE = 3, FNT_ENABLE = 0, FNT_STATUS = 1,
			FNT_PARAM_UP = 2, FNT_PARAM_DOWN = 3, FNT_HP_LOW = 4, FNT_MP_LOW = 4,
		};

		static const int FONT_H = 12;
		static const int FULL_FONT_W = 12;
		static const int HALF_FONT_W = FULL_FONT_W / 2;

		class Font {
		private:
			static const int TEN_MAX = 94;
			static const int  KU_MAX = 94;

			static const uint8_t  HANKAKU[0x100][FONT_H];

			static const uint16_t GOTHIC[KU_MAX+1][TEN_MAX+1][FONT_H];
			static const uint16_t MINCHO[KU_MAX+1][TEN_MAX+1][FONT_H];

			static const uint16_t EXTERNAL_UP['Z'-'A' + 1][FONT_H];
			static const uint16_t EXTERNAL_LW['z'-'a' + 1][FONT_H];

			static void getTen(int& ku, int& ten, uint8_t sbyte);
		public:
			static const void* getGothic(const char* input);
			static const void* getMincho(const char* input);

			static const uint16_t* getExternalChar(char c);

			static bool isFullChar(const char* input);
			static bool isHalfChar(const char* input);

			static int width(const std::string& str) { return str.length() * HALF_FONT_W; }
		};

	};
};

#endif
