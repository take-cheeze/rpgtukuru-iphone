#include "Define.hpp"

const char* rpg2k::define::EventState = (" \
Array1D EventState \n \
{ \n \
	[11]: int mapID; \n \
	[12]: int x = 0; \n \
	[13]: int y = 0; \n \
 \n \
	[21]: int defDir; \n \
	[22]: int talkDir; \n \
	[23]: int dummy; \n \
	[24]: int dummy; \n \
 \n \
	[31]: int action = 0; \n \
	[32]: int freq; \n \
	[33]: int startType = 0; \n \
	[34]: int priorityType; \n \
	[35]: int dummy; \n \
	[36]: int animeType = 0; \n \
	[37]: int speed = 3; \n \
 \n \
	[41]: Array1D Move \n \
	{ \n \
		[11]: int length = 0; \n \
		[12]: Binary data; \n \
\n \
		[21]: bool repeat = true; \n \
		[22]: bool pass = false; \n \
	}; \n \
	[43]: int dummy; \n \
	[46]: bool isTrans = false; \n \
 \n \
	// [51]: \n \
	[52]: int dummy; \n \
	[53]: int dummy; \n \
	[54]: int dummy; \n \
 \n \
	[71]: int dummy; \n \
	// [72]: \n \
	[73]: string charSet; \n \
	[74]: int charSetPos = 0; \n \
	[75]: int charSetPat = 1; \n \
\n \
	[81]: int dummy; \n \
	[82]: int dummy; \n \
	[83]: int dummy; \n \
\n \
	[101]: int dummy; \n \
	[103]: int dummy; \n \
	[108]: Array1D dummy \n \
	{ \n \
		[1]: int dummy; \n \
	}; \n \
 \n \
	[121]: int dummy; \n \
 \n \
	[131]: int dummy; \n \
	[132]: Binary dummy; \n \
}; \n \
");
