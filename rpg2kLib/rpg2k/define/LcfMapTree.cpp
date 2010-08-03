#include "Define.hpp"

const char* rpg2k::define::LcfMapTree = (" \
Array2D Grobal \n \
{ \n \
	[1]: string name = \"\"; \n \
	[2]: int parentMapID; \n \
 \n \
	[3]: int type; \n \
	[4]: int type; // copy of [3] ? \n \
	[5]: int unknown0; \n \
	[6]: int unknown1; \n \
	[7]: bool isOpen = false; // relate with nest? \n \
 \n \
	[11]: int musicType; \n \
	[12]: Music music; \n \
 \n \
	[21]: int backdropType; \n \
	[22]: string backdrop; \n \
 \n \
	[31]: int teleport; \n \
	[32]: int escape; \n \
	[33]: int save; \n \
 \n \
	[41]: Array2D Encount \n \
	{ \n \
		[1]: int enemyGroupID; \n \
	}; \n \
	[44]: int encuntStep = 25; \n \
 \n \
	[51]: Binary areaRange; // uint32_t[4] = { startx, starty, endx, endy }; \n \
}; \n \
 \n \
BerEnum mapExist; \n \
 \n \
Array1D StartPoint \n \
{ \n \
	[1]: int partyMapID; \n \
	[2]: int partyX = 0; \n \
	[3]: int partyY = 0; \n \
 \n \
	[11]: int boatMapID = 0; \n \
	[12]: int boatX = 0; \n \
	[13]: int boatY = 0; \n \
 \n \
	[21]: int shipMapID = 0; \n \
	[22]: int shipX = 0; \n \
	[23]: int shipY = 0; \n \
 \n \
	[31]: int airshipMapID = 0; \n \
	[32]: int airshipX = 0; \n \
	[33]: int airshipY = 0; \n \
}; \n \
");
