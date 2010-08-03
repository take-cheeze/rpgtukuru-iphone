#include "Define.hpp"

const char* rpg2k::define::LcfMapUnit = (" \
Array1D Grobal \n \
{ \n \
	[1]: int chipSetID = 1; \n \
	[2]: int width     = 20; \n \
	[3]: int height    = 15; \n \
 \n \
	[11]: int scrollType; \n \
 \n \
	[31]: bool usePanorama = false; \n \
	[32]: string panorama; \n \
	[33]: bool scrollPanoramaH = false; \n \
	[34]: bool scrollPanoramaV = false; \n \
	[35]: bool autoScrollPanoramaH = false; \n \
	[36]: int autoScrollPanoramaSpeedH = 0; \n \
	[37]: bool autoScrollPanoramaV = false; \n \
	[38]: int autoScrollPanoramaSpeedV = 0; \n \
 \n \
	[71]: Binary lower; \n \
	[72]: Binary upper; \n \
 \n \
	[81]: Array2D Event \n \
	{ \n \
		[1]: string name; \n \
		[2]: int x = 0; \n \
		[3]: int y = 0; \n \
		// [4]: \n \
		[5]: Array2D Page \n \
		{ \n \
			[2]: Array1D Term \n \
			{ \n \
				[1]: int flag = 0; \n \
				[2]: int switchID1 = 1; \n \
				[3]: int switchID2 = 1; \n \
				[4]: int variableID = 1; \n \
				[5]: int value = 0; \n \
				[6]: int itemID = 1; \n \
				[7]: int charID = 1; \n \
				[8]: int timerLeft = 0; \n \
			}; \n \
 \n \
			[21]: string charSet; \n \
			[22]: int charSetPos = 0; \n \
			[23]: int charSetdir = 2; \n \
			[24]: int charSetPat = 1; \n \
			[25]: bool semiTrans = false; \n \
 \n \
			[31]: int action = 0; \n \
			[32]: int freq = 3; \n \
			[33]: int startType = 0; \n \
			[34]: int priorityType = 0; \n \
			[35]: bool noPile = false; \n \
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
 \n \
			[51]: int eventLength = 0; \n \
			[52]: Event event; \n \
		}; \n \
	}; \n \
 \n \
	[91]: int saveTime; \n \
}; \n \
");
