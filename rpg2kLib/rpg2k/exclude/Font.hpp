#ifndef _INC__RPG2k__FONT_FONT_HPP
#define _INC__RPG2k__FONT_FONT_HPP

#include "Define.hpp"


namespace rpg2k
{
	namespace font
	{
		enum CharType { HALF = 0, FULL, };
		namespace Face { enum Type { GOTHIC = 0, MINCHO, }; }
		enum FontColor {
			FNT_NORMAL = 0, FNT_DISABLE = 3, FNT_ENABLE = 0, FNT_STATUS = 1,
			FNT_MONEY = 1,
			FNT_PARAM_UP = 2, FNT_PARAM_DOWN = 3, FNT_HP_LOW = 4, FNT_MP_LOW = 4,
		};

		static int const FONT_H = 12;
		static int const FULL_FONT_W = 12;
		static int const HALF_FONT_W = FULL_FONT_W / 2;

		typedef uint16_t const font_t;

		// all input encoding type must be Shift_JIS
		class Font
		{
		private:
			static int const TEN_MAX = 94;
			static int const  KU_MAX = 94;

			static font_t HANKAKU[0x100][FONT_H];

			static font_t GOTHIC[KU_MAX][TEN_MAX][FONT_H];
			static font_t MINCHO[KU_MAX][TEN_MAX][FONT_H];

			static font_t EXTERNAL_UP['Z'-'A' + 1][FONT_H];
			static font_t EXTERNAL_LW['z'-'a' + 1][FONT_H];

			static font_t* getFullFrom(RPG2kString::const_iterator& it, font_t (&src)[KU_MAX][TEN_MAX][FONT_H]);
			static font_t* getHalfFrom(RPG2kString::const_iterator& it, font_t (&src)[0x100][FONT_H]);
		public:
			static font_t* getGothic(RPG2kString::const_iterator& it) { return getFullFrom(it, Font::GOTHIC); }
			static font_t* getMincho(RPG2kString::const_iterator& it) { return getFullFrom(it, Font::MINCHO); }

			/*
			 * no char check at these function
			 * check with check() before using this
			 */
			static font_t* getFull(RPG2kString::const_iterator& it, Face::Type t);
			static font_t* getHalf(RPG2kString::const_iterator& it) { return getHalfFrom(it, HANKAKU); }

			static font_t* getExternal(RPG2kString::const_iterator& it);

			/*
			 * judges the char type
			 */
			static CharType check(RPG2kString::const_iterator const& it);
			static bool isExternal(RPG2kString::const_iterator const& it);

			static uint width(RPG2kString const& str);
		}; // class Font
	} // namespace font
} // namespace rpg2k

#endif
