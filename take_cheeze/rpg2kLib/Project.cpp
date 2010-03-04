#include "Debug.hpp"
#include "Encode.hpp"
#include "Project.hpp"

using namespace rpg2kLib::debug;
using namespace rpg2kLib::encode;
using namespace rpg2kLib::structure;


namespace rpg2kLib
{
	namespace model
	{

Project::Project(std::string baseDir, std::string rtpDir)
: BASE_DIR(baseDir), RTP_DIR(rtpDir)
, LDB(baseDir), LMT(baseDir)
{
	init();
}
/*
Project::Project(Main& m)
: BASE_DIR( m.getGameDir() ), RTP_DIR( m.getRTPDir() )
, LDB(BASE_DIR), LMT(BASE_DIR)
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

/*
Array1D& Project::getLMT()
{
	return getLMT( getCurrentMapID() );
}
 */
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
	return LSD[0][101].getArray1D().exists(121)
		? getLSD()[101].getArray1D()[121]
		: getLMT().canTeleport( getCurrentMapID() );
}
bool Project::canEscape()
{
	return LSD[0][101].getArray1D().exists(122)
		? getLSD()[101].getArray1D()[122]
		: getLMT().canEscape( getCurrentMapID() );
}
bool Project::canSave()
{
	return false;

	return LSD[0][101].getArray1D().exists(123)
		? getLSD()[101].getArray1D()[123]
		: getLMT().canSave( getCurrentMapID() );
}
bool Project::canOpenMenu()
{
	return LSD[0][101].getArray1D().exists(124)
		? getLSD()[101].getArray1D()[124]
		: true;
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
	std::vector< uint16_t > data = chipSet[3].getBinary();

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

	for(Array2D::ReverseIterator it = pages.rbegin(); it != pages.rend(); ++it) {
		if( getLSD().validPageMap( it.second()[2] ) ) return it.second().getIndex();
	}

	throw "Page not found.";
}

bool Project::  equip(uint charID, uint itemID)
{
	DataBase& ldb = getLDB();
	SaveData& lsd = getLSD();
	int itemNum = lsd.getItemNum(itemID);

	if(itemNum == 0) return false;

	uint type = ldb[13].getArray2D()[itemID][3];
	switch(type) {
		case 1: case 2: case 3: case 4: case 5:
			type--; break;
		default:
			throw "Not a equipable item.";
	}

	std::vector< uint16_t > vec = lsd.charParam(charID)[61].getBinary();
	vec[type] = itemID;
	lsd.charParam(charID)[61].getBinary() = vec;

	lsd.setItemNum(itemID, --itemNum);

	return true;
}
void Project::unequip(uint charID, EquipType type)
{
	SaveData& lsd = getLSD();

	std::vector< uint16_t > vec = lsd.charParam(charID)[61].getBinary();

	lsd.setItemNum( vec[type], lsd.getItemNum(vec[type]+1) );
	vec[type] = 0;

	lsd.charParam(charID)[61].getBinary() = vec;
}

string Project::systemGraphic()
{
	Array1D& sys = getLSD()[101];

	if( sys.exists(21) ) return sys[21];
	else return getLDB()[22].getArray1D()[22];
}
uint Project::wallpaperType()
{
	Array1D& sys = getLSD()[101];

	if( sys.exists(22) ) return sys[22];
	else return getLDB()[22].getArray1D()[71];
}
uint Project::fontType()
{
	Array1D& sys = getLSD()[101];

	if( sys.exists(23) ) return sys[23];
	else return getLDB()[22].getArray1D()[72];
}

	}; // namespace model
}; // namespace rpg2kLib
