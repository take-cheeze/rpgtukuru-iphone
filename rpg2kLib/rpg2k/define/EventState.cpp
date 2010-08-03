#include "Define.hpp"

const char* rpg2k::define::EventState = (" \
Array1D EventState \n \
{ \n \
	[11]: int mapID = 0; \n \
	[12]: int x = 0; \n \
	[13]: int y = 0; \n \
 \n \
	[21]: int defDir; \n \
	[22]: int talkDir; \n \
 \n \
	[30]: int dummy = 0; \n \
	[31]: int dummy = 0; \n \
	[32]: int charDown  = 2; \n \
	[33]: int dummy = 0; \n \
	[34]: int charLeft  = 3; \n \
	[35]: int dummy = 0; \n \
	[36]: int charRight = 1; \n \
	[37]: int dummy = 0; \n \
	[38]: int charUp    = 0; \n \
 \n \
	[41]: Array1D dummy \n \
	{ \n \
	}; \n \
	[46]: bool isTrans = false; \n \
	// [51]: \n \
	[52]: int dummy; \n \
	[53]: int dummy; \n \
	[54]: int dummy; \n \
 \n \
	// [71]: \n \
	// [72]: \n \
	[73]: string charSet; \n \
	[74]: int charSetPos = 0; \n \
	[75]: int charSetPat = 1; \n \
 \n \
	[108]: Array1D dummy \n \
	{ \n \
	}; \n \
}; \n \
");
