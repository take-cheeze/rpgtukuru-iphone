#include "Define.hpp"

const char* rpg2k::define::LcfSaveData = (" \
Array1D Grobal \n \
{ \n \
	[100]: Array1D Preview \n \
	{ \n \
		[1]: Binary timeStamp; \n \
 \n \
		[11]: string name; \n \
		[12]: int level = 1; \n \
		[13]: int HP; \n \
 \n \
		[21]: string faceSet1; \n \
		[22]: int faceSetPos1 = 0; \n \
		[23]: string faceSet2; \n \
		[24]: int faceSetPos2 = 0; \n \
		[25]: string faceSet3; \n \
		[26]: int faceSetPos3 = 0; \n \
		[27]: string faceSet4; \n \
		[28]: int faceSetPos4 = 0; \n \
	}; \n \
 \n \
	[101]: Array1D System \n \
	{ \n \
/* \n \
		[1]: \n \
 \n \
		[11]: \n \
*/ \n \
		[21]: string system; \n \
		[22]: int wallpaper; \n \
		[23]: int font; \n \
 \n \
		[31]: int switchDataNum = 0; \n \
		[32]: Binary switchData; \n \
		[33]: int variableDataNum = 0; \n \
		[34]: Binary variableData; \n \
 \n \
		[41]: int windowType = 0; \n \
		[42]: int windowViewPoint = 2; \n \
		[43]: bool doNotHideParty = true; \n \
		[44]: bool moveAtWait = false; \n \
 \n \
		[51]: string faceSet; \n \
		[52]: int faceSetPos = 0; \n \
		[53]: int side = 0; \n \
		[54]: bool flip = false; \n \
		// [55]: \n \
 \n \
		[71]: Music dummy; \n \
		[72]: Music battle; \n \
		[73]: Music battleEnd; \n \
		[74]: Music hotel; \n \
		[75]: Music current; \n \
		[76]: Music dummy; \n \
		[77]: Music dummy; \n \
		[78]: Music memorized; \n \
		[79]: Music boat; \n \
		[80]: Music ship; \n \
		[81]: Music airShip; \n \
		[82]: Music gameOver; \n \
 \n \
		[ 91]: Sound cursorMove; \n \
		[ 92]: Sound enter; \n \
		[ 93]: Sound cancel; \n \
		[ 94]: Sound buzzer; \n \
		[ 95]: Sound battleStart; \n \
		[ 96]: Sound runAway; \n \
		[ 97]: Sound enemyAttack; \n \
		[ 98]: Sound enemyDamaged; \n \
		[ 99]: Sound partyDamaged; \n \
		[100]: Sound evasion; \n \
		[101]: Sound enamyDead; \n \
		[102]: Sound itemUse; \n \
 \n \
		[111]: Binary moveErase; \n \
		[112]: Binary moveShow; \n \
		[113]: Binary battleStartErase; \n \
		[114]: Binary battleStartShow; \n \
		[115]: Binary battleEndErase; \n \
		[116]: Binary battleEndShow; \n \
 \n \
		[121]: bool canTeleport; \n \
		[122]: bool canEscape; \n \
		[123]: bool canSave; \n \
		[124]: bool canOpenMenu; \n \
		[125]: string backdrop; \n \
 \n \
		[131]: int saveTime; \n \
	}; \n \
 \n \
	[102]: Array1D dummy \n \
	{ \n \
		[1]: Binary dummy; \n \
		[11]: Binary dummy; \n \
 \n \
		[31]: Binary dummy; \n \
		[32]: Binary dummy; \n \
 \n \
		[41]: Binary dummy; \n \
		[42]: Binary dummy; \n \
		[43]: Binary dummy; \n \
		[44]: Binary dummy; \n \
		[45]: Binary dummy; \n \
 \n \
		[49]: Binary dummy; \n \
	}; \n \
 \n \
	[103]: Array2D Picture \n \
	{ \n \
		[1]: string picture; \n \
		[2]: double startX = 160; \n \
		[3]: double startY = 120; \n \
		[4]: double currentX = 160; \n \
		[5]: double currentY = 120; \n \
		[6]: bool scroll = false; \n \
		[8]: double dummy; \n \
		[9]: bool trans = false; \n \
 \n \
		[11]: double red; \n \
		[12]: double green; \n \
		[13]: double blue; \n \
		[14]: double chroma; \n \
		[15]: int effect; \n \
		[16]: double speedOrPower; \n \
 \n \
		[31]: double goalX; \n \
		[32]: double goalY; \n \
		[33]: int magnify = 100; \n \
		[34]: int trans = 0; \n \
 \n \
		[41]: int red   = 100; \n \
		[42]: int green = 100; \n \
		[43]: int blue  = 100; \n \
		[44]: int chroma = 100; \n \
		[46]: int speedOrPower; \n \
 \n \
		[51]: int countDown = 0; \n \
		[52]: double degree; \n \
		[53]: int dummy; \n \
	}; \n \
 \n \
	[104]: EventState Party  ; \n \
	[105]: EventState Boat   ; \n \
	[106]: EventState Ship   ; \n \
	[107]: EventState Airship; \n \
 \n \
	[108]: Array2D Character \n \
	{ \n \
		[1]: string name; \n \
		[2]: string title; \n \
 \n \
		[11]: string charSet; \n \
		[12]: int charSetPos; \n \
 \n \
		[21]: string faceSet; \n \
		[22]: int faceSetPos; \n \
 \n \
		[31]: int level; \n \
		[32]: int exp; \n \
		[33]: int HP; \n \
		[34]: int MP; \n \
 \n \
		[41]: int attack = 0; \n \
		[42]: int gaurd  = 0; \n \
		[43]: int mind   = 0; \n \
		[44]: int speed  = 0; \n \
 \n \
		[51]: int skillDataNum = 0; \n \
		[52]: Binary skillData; // vector< uint16_t > \n \
 \n \
		[61]: Binary equipment; // vector< uint16_t > \n \
 \n \
		[71]: int curHP = 0; \n \
		[72]: int curMP = 0; \n \
 \n \
		[81]: int conditionDataNum = 0; \n \
		[82]: Binary condition; \n \
 \n \
		[90]: int job; \n \
		[91]: bool doubleHand; \n \
	}; \n \
 \n \
	[109]: Array1D Status \n \
	{ \n \
		[1]: int memberNum = 0; \n \
		[2]: Binary member; \n \
 \n \
		[11]: int itemTypeNum = 0; \n \
		[12]: Binary itemID; \n \
		[13]: Binary itemNum; \n \
		[14]: Binary ItemUse; \n \
 \n \
		[21]: int money = 0; \n \
		// [23]: \n \
 \n \
		[32]: int battle = 0; \n \
		[33]: int loss = 0; \n \
		[34]: int win = 0; \n \
		[35]: int escape = 0; \n \
 \n \
		// [41]: \n \
		// [42]: \n \
	}; \n \
 \n \
	[110]: Array2D Teleport \n \
	{ \n \
		[1]: int mapID = 0; \n \
		[2]: int x = 0; \n \
		[3]: int y = 0; \n \
		[4]: bool onAfterTeleport = false; \n \
		[5]: int switchID = 1; \n \
	}; \n \
 \n \
	[111]: Array1D EventData \n \
	{ \n \
		[3]: int encountRate; \n \
 \n \
		[5]: int chipSetID; \n \
 \n \
		[11]: Array2D info = EventState; \n \
 \n \
		[21]: Binary replaceLower; \n \
		[22]: Binary replaceUpper; \n \
 \n \
		[31]: bool usePanorama; \n \
		[32]: string panorama; \n \
		[33]: bool horizontalScroll; \n \
		[34]: bool verticalScroll; \n \
		[35]: bool horizontalScrollAuto; \n \
		[36]: int horizontalScrollSpeed; \n \
		[37]: bool verticalScrollAuto; \n \
		[38]: int verticalScrollSpeed; \n \
	}; \n \
 \n \
	[112]: Array1D dummy \n \
	{ \n \
	}; \n \
 \n \
	[113]: Array1D dummy \n \
	{ \n \
		[1]: Array2D dummy \n \
		{ \n \
			[1]: int eventLength; \n \
			[2]: Event event; \n \
 \n \
			// [11]: \n \
			// [12]: \n \
			// [13]: \n \
 \n \
			// [21]: \n \
			// [22]: \n \
 \n \
			// [31]: \n \
		}; \n \
	}; \n \
 \n \
	[114]: Array2D dummy \n \
	{ \n \
		[1]: Array1D dummy \n \
		{ \n \
		}; \n \
	}; \n \
 \n \
}; \n \
");
