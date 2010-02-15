#include "Debug.hpp"
#include "Encode.hpp"
#include "Project.hpp"

using namespace rpg2kLib::debug;
using namespace rpg2kLib::encode;
using namespace rpg2kLib::model;

Project::Project(string baseDir, string rtpDir)
	: BASE_DIR(baseDir), RTP_DIR(rtpDir),
		LDB(baseDir), LMT(baseDir), LMU(), LSD()
{
	init();
}
/*
Project::Project(Main& m)
	: BASE_DIR( m.getGameDir() ), RTP_DIR( m.getRTPDir() ),
		LDB(BASE_DIR), LMT(BASE_DIR), LMU(), LSD()
{
	init();
}
 */
void Project::init()
{
// LcfSaveData
	LAST_SAVEDATA_STAMP = 0;
	LAST_LSD_ID = ID_MIN;

	for(uint i = ID_MIN; i <= SAVE_DATA_MAX; i++) {
		try {
			LSD.add(i, *new SaveData(BASE_DIR, i) );

			Binary& bin = ( (Array1D&) (LSD[i][100]) )[1];
			uint64_t cur = *( (uint64_t*) bin.getPtr() );
			if(cur > LAST_SAVEDATA_STAMP) {
				LAST_LSD_ID = i;
				LAST_SAVEDATA_STAMP = cur;
			}
		} catch(...) {}
	}
// set LcfSaveData buffer
	LSD.add( 0, *new SaveData() );
}

Project::~Project()
{
}

uint Project::getCurrentMapID()
{
	return getLSD().eventState(EV_ID_PARTY).mapID();
}

Array1D& Project::getLMT()
{
	return getLMT( getCurrentMapID() );
}
MapUnit & Project::getLMU()
{
	return getLMU( getCurrentMapID() );
}
MapUnit & Project::getLMU(uint id)
{
	if( !LMU.exists(id) ) LMU.add( id, *new MapUnit(BASE_DIR, id) );
	return LMU[id];
}
SaveData& Project::getLSD()
{
	return LSD[ID_MIN-1];
}
SaveData& Project::getLSD(uint id)
{
	if( (ID_MIN <= id) && (id <= SAVE_DATA_MAX) ) return LSD[id];
	else throw "Invalid saveDataID";
}

bool Project::canTeleport()
{
	if( static_cast< Array1D& >(LSD[0][101]).exists(121) )
		return static_cast< Array1D& >(LSD[0][101])[121];
	else return true;
}
bool Project::canEscape()
{
	if( ( (Array1D&) LSD[0][101] ).exists(122) )
		return ( (Array1D&) LSD[0][101] )[122];
	else return true;
}
bool Project::canSave()
{
	return false;

	if( ( (Array1D&) LSD[0][101] ).exists(123) )
		return ( (Array1D&) LSD[0][101] )[123];
	else return true;
}
bool Project::canOpenMenu()
{
	if( static_cast< Array1D& >(LSD[0][101]).exists(124) )
		return static_cast< Array1D& >(LSD[0][101])[124];
	else return true;
}

void Project::loadLSD(uint id)
{
	if( id == (ID_MIN - 1) ) {
/*
		Array1D& start = PROJECT.LMT.getStartPoint();

		charX = start[2];
		charY = start[3];
		charDir = material::CHAR_DOWN;
*/
		return;
	}

	if(SAVE_DATA_MAX < id) throw "Invalid save data ID.";

	LSD[0] = LSD[id];
}
void Project::saveLSD(uint id)
{
	if(SAVE_DATA_MAX < id) throw "Invalid save data ID.";

	LSD[id] = LSD[0];
	LSD[id].save();
}

bool Project::isBelow(uint chipID)
{
	Array1D& chipSet = ( (Array2D&)LDB[20] )[chipSetID()];
	Binary& lower = chipSet[4]; Binary& upper = chipSet[5];

	int index;

	if( (0 <= chipID) && (chipID < 3000) ) { index =  0 + chipID/1000;
	} else if(chipID == 3028) { index =  3 + 0;
	} else if(chipID == 3078) { index =  3 + 1;
	} else if(chipID == 3128) { index =  3 + 2;
	} else if( ( 4000 <= chipID) && (chipID <  5000) ) {
		index = 6 + (chipID-4000)/50;
		if( (lower[index] & 0x30) == 0x30 ) {
			switch( (chipID-4000) % 50 ) {
				case 0x14: case 0x15: case 0x16: case 0x17:
				case 0x21: case 0x22: case 0x23: case 0x24: case 0x25:
				case 0x2a: case 0x2b:
				case 0x2d: case 0x2e:
					return false;
				default: return true;
			}
		}
	} else if( ( 5000 <= chipID) && (chipID <  5144) ) {
			index = 18 + chipID-5000;
	} else if( (10000 <= chipID) && (chipID < 10144) ) {
		return (upper[chipID-10000] & 0x30) == 0x00;
	} else throw "Invalid chipSetID;"; // + chipSetID;

	return (lower[index] & 0x30) == 0x00;
}
bool Project::isAbove(uint chipID)
{
	Array1D& chipSet = ( (Array2D&)LDB[20] )[chipSetID()];
	Binary& lower = chipSet[4]; Binary& upper = chipSet[5];

	int index;

	if( (0 <= chipID) && (chipID < 3000) ) { index =  0 + chipID/1000;
	} else if(chipID == 3028) { index =  3 + 0;
	} else if(chipID == 3078) { index =  3 + 1;
	} else if(chipID == 3128) { index =  3 + 2;
	} else if( ( 4000 <= chipID) && (chipID <  5000) ) {
		index =  6 + (chipID-4000)/50;
		if( (lower[index] & 0x30) == 0x30 ) {
			switch((chipID-4000)%50) {
				case 0x14: case 0x15: case 0x16: case 0x17:
				case 0x21: case 0x22: case 0x23: case 0x24: case 0x25:
				case 0x2a: case 0x2b:
				case 0x2d: case 0x2e:
					return true;
				default: return false;
			}
		}
	} else if( ( 5000 <= chipID) && (chipID <  5144) ) {
		index = 18 + getLSD().getReplace(CHIP_LOWER, chipID-5000);
	} else if( (10000 <= chipID) && (chipID < 10144) ) {
		return (upper[getLSD().getReplace(CHIP_UPPER, chipID-10000)] & 0x30) == 0x10;
	} else throw "Invalid chipSetID;"; // + chipSetID;

	return (lower[index] & 0x30) == 0x10;
}
bool Project::isCounter(uint chipID)
{
	Array1D& chipSet = ( (Array2D&)LDB[20] )[chipSetID()];
	Binary& upper = chipSet[5];

	if((10000 <= chipID) && (chipID < 10144))
		return (upper[chipID-10000] & 0x40) != 0x00;
	else return false;
}
uint8_t Project::getPass(uint chipID)
{
	Array1D& chipSet = ( (Array2D&)LDB[20] )[chipSetID()];
	Binary& lower = chipSet[4]; Binary& upper = chipSet[5];

	int index;

	if((0 <= chipID) && (chipID < 3000)) index = 0 + chipID/1000;
	else if(chipID == 3028) index = 3 + 0;
	else if(chipID == 3078) index = 3 + 1;
	else if(chipID == 3128) index = 3 + 2;
	else if( ( 4000 <= chipID) && (chipID <  5000) )
		index =  6 + (chipID-4000)/50;
	else if( ( 5000 <= chipID) && (chipID <  5144) )
		index = 18 +  getLSD().getReplace(CHIP_LOWER, chipID-5000);
	else if( (10000 <= chipID) && (chipID < 10144) )
		return upper[getLSD().getReplace(CHIP_UPPER, chipID-10000)];
	else throw "Invalid chipSetID;"; // + chipSetID;

	return lower[index];
}
int Project::getTerrainID(uint chipID)
{
	Array1D& chipSet = ( (Array2D&)LDB[20] )[chipSetID()];
	vector< uint16_t > data = static_cast< Binary& >(chipSet[3]);

	int index;

	if((0 <= chipID) && (chipID < 3000)) index = 0 + chipID/1000;
	else if(chipID == 3028) index = 3 + 0;
	else if(chipID == 3078) index = 3 + 1;
	else if(chipID == 3128) index = 3 + 2;
	else if( (4000 <= chipID) && (chipID < 5000) ) index =  6 + (chipID-4000)/50;
	else if( (5000 <= chipID) && (chipID < 5144) )
		index = 18 + getLSD().getReplace(CHIP_LOWER, chipID-5000);
	else throw "Invalid chipSetID;"; // + chipSetID;

	return data[index];
}

uint Project::currentPageID(uint eventID)
{
	Array2D& pages = ( (Array2D&) getLMU()[81] )[eventID][5];

	Array2D::Iterator it = pages.end();
	if( it == pages.begin() ) throw "Page not found.";
	while(true) {
		--it;
		if( it == pages.begin() ) throw "Page not found.";

		if( it.second().exists() && getLSD().validPageMap( it.second()[2] ) ) {
			return it.second().getIndex();
		}
	}

	throw "Page not found.";
}

bool Project::  equip(uint charID, uint itemID)
{
	DataBase& ldb = getLDB();
	SaveData& lsd = getLSD();
	int itemNum = lsd.getItemNum(itemID);

	if(itemNum == 0) return false;

	uint type = static_cast<Array2D&>(ldb[13])[itemID][3];
	switch(type) {
		case 1: case 2: case 3: case 4: case 5:
			type--; break;
		default:
			throw "Not a equipable item.";
	}

	vector< uint16_t > vec =
		static_cast< Binary& >( lsd.charParam(charID)[61] );
	vec[type] = itemID;
	static_cast< Binary& >( lsd.charParam(charID)[61] ) = vec;

	lsd.setItemNum(itemID, --itemNum);

	return true;
}
void Project::unequip(uint charID, EquipType type)
{
	SaveData& lsd = getLSD();

	vector< uint16_t > vec =
		static_cast< Binary& >( lsd.charParam(charID)[61] );

	lsd.setItemNum( vec[type], lsd.getItemNum(vec[type]+1) );
	vec[type] = 0;

	static_cast< Binary& >( lsd.charParam(charID)[61] ) = vec;
}

string Project::systemGraphic()
{
	Array1D& sys = getLSD()[101];

	if( sys.exists(21) ) return sys[21];
	else return static_cast< Array1D& >( getLDB()[22] )[22];
}
uint Project::wallpaperType()
{
	Array1D& sys = getLSD()[101];

	if( sys.exists(22) ) return sys[22];
	else return static_cast< Array1D& >( getLDB()[22] )[71];
}
uint Project::fontType()
{
	Array1D& sys = getLSD()[101];

	if( sys.exists(23) ) return sys[23];
	else return static_cast< Array1D& >( getLDB()[22] )[72];
}
