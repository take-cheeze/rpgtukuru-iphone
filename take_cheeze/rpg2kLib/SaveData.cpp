#include <algorithm>
#include <iomanip>
#include <sstream>

#include "SaveData.hpp"

using namespace rpg2kLib::structure;


namespace rpg2kLib
{
	namespace model
	{

typedef std::map< uint16_t, SaveData::Item >::const_iterator item_it;

SaveData::SaveData(std::string dir, std::string name)
: Base(dir, name), ID(0)
{
	init();
}
SaveData::SaveData(std::string dir, uint id)
: Base(dir, ""), ID(id)
{
	std::ostringstream ss;
	ss << "Save" << std::setfill('0') << std::setw(2) << id << ".lsd";
	setFileName( ss.str() );

	checkExists();

	if( !exists() ) {
		throw "Save data doesn`t exist.";
	}

	init();

}
void SaveData::init()
{
	if( ( getID() < ID_MIN ) || ( SAVE_DATA_MAX < getID() ) )
		throw "Invalid save data ID.";

	Base::load();

	Array1D& status = (*this)[109];
	Array1D& sys    = (*this)[101];

// item
	{
		int itemTypeNum = status[11];
		std::vector< uint16_t > id  = status[12].getBinary();
		std::vector< uint8_t  > num = status[13].getBinary();
		std::vector< uint8_t  > use = status[14].getBinary();

		for(int i = 0; i < itemTypeNum; i++) {
			Item item = { num[i], use[i] };
			ITEM.insert( std::map< uint16_t, Item >::value_type(id[i], item) );
		}
	}
// switch and variable
	SWITCH.resize(sys[31]);
	SWITCH = sys[32].getBinary();
	VARIABLE.resize(sys[33]);
	VARIABLE = sys[34].getBinary();
// member
	MEMBER.resize(status[1]);
	MEMBER = status[2].getBinary();
// chip replace
	CHIP_REPLACE.resize(CHIP_TYPE_NUM);
	for(uint i = 0; i < CHIP_TYPE_NUM; i++) {
		CHIP_REPLACE[i].resize(CHIP_REPLACE_MAX);
		CHIP_REPLACE[i] = ( (Array1D&) (*this)[111] )[21+i].getBinary();
	}
}

SaveData::~SaveData()
{
	std::cout << getHeader() << std::endl;
}

void SaveData::save()
{
	Array1D& status = (*this)[109];
	Array1D& sys = (*this)[101];

// item
	{
		int itemNum = ITEM.size();
		status[11] = itemNum;

		std::vector< uint16_t > id (itemNum);
		std::vector< uint8_t  > num(itemNum);
		std::vector< uint8_t  > use(itemNum);

		int i = 0;
		for(item_it it = ITEM.begin(); it != ITEM.end(); ++it) {
			id [i] = it->first;
			num[i] = it->second.num;
			use[i] = it->second.use;

			i++;
		}
		status[12].getBinary() = id ;
		status[13].getBinary() = num;
		status[14].getBinary() = use;
	}
// switch and variable
	sys[31] = SWITCH.size();
	sys[32].getBinary() = SWITCH;
	sys[33] = VARIABLE.size();
	sys[34].getBinary() = VARIABLE;
// member
	( (Array1D&)(*this)[109] )[1] = MEMBER.size();
	( (Array1D&)(*this)[109] )[2].getBinary() = MEMBER;
// chip replace
	for(uint i = 0; i < CHIP_TYPE_NUM; i++)
		( (Array1D&) (*this)[111] )[21+i].getBinary() = CHIP_REPLACE[i];

	Base::save();
}

SaveData& SaveData::operator =(SaveData& src)
{
	this->getData() = src.getData();
	return *this;
}

bool SaveData::getFlag(uint id)
{
	return ( id < SWITCH.size() ) ? SWITCH[id] : SWITCH_DEF_VAL;
}
void SaveData::setFlag(uint id, bool data)
{
	if( id >= SWITCH.size() ) SWITCH.resize(id+1, SWITCH_DEF_VAL);
	SWITCH[id] = data;
}

int32_t SaveData::getVar(uint id)
{
	return ( id < VARIABLE.size() ) ? VARIABLE[id] : VAR_DEF_VAL;
}
void    SaveData::setVar(uint id, int32_t data)
{
	if( id >= VARIABLE.size() ) VARIABLE.resize(id+1, VAR_DEF_VAL);
	VARIABLE[id] = data;
}

int SaveData::getMoney()
{
	return (*this)[109].getArray1D()[21];
}
void SaveData::setMoney(int data)
{
	if(data < MONEY_MIN) data = MONEY_MIN;
	else if(MONEY_MAX < data) data = MONEY_MAX;

	(*this)[109].getArray1D()[21] = data;
}

uint SaveData::getItemNum(uint id)
{
	return ( ITEM.find(id) == ITEM.end() ) ? 0 : ITEM[id].num;
}
void SaveData::setItemNum(uint id, uint val)
{
	if(val < ITEM_MIN) val = MONEY_MIN;
	else if(ITEM_MAX < val) val = MONEY_MAX;

	if( ITEM.find(id) == ITEM.end() ) {
		Item i = { val, 0 };
		ITEM.insert( std::map< uint, Item >::value_type(id, i) );
	} else {
		ITEM[id].num = val;
	}
}

uint SaveData::getItemUse(uint id)
{
	return ( ITEM.find(id) == ITEM.end() ) ? 0 : ITEM[id].use;
}
void SaveData::setItemUse(uint id, uint val)
{
	if( ITEM.find(id) != ITEM.end() ) ITEM[id].use = val;
}

bool SaveData::hasItem(uint id)
{
	if( ( ITEM.find(id) != ITEM.end() ) && (ITEM[id].num != 0) ) return true;
	else {
		for(uint i = 0; i < MEMBER.size(); i++) {
			std::vector< uint16_t > equip = charParam(MEMBER[i])[61].getBinary();

			for(int j = 0; j < EQUIP_NUM; j++) if(equip[i] == id) return true;
		}
	}

	return false;
}

bool SaveData::validPageMap(Array1D& term)
{
	int flags = term[1];

	return (
		( ( flags & (0x01 << 0) ) && !this->getFlag(term[2]) ) ||
		( ( flags & (0x01 << 1) ) && !this->getFlag(term[3]) ) ||
		( ( flags & (0x01 << 2) ) &&
			(this->getVar (term[4]) < term[5].get_int() )
		) || (
			( flags & (0x01 << 3) ) && !this->hasItem(term[6]) ) ||
		(
			( flags & (0x01 << 4) ) &&
			( find( MEMBER.begin(), MEMBER.end(), (uint)term[7] ) == MEMBER.end() )
		) || (
			( flags & (0x01 << 5) ) && (this->timerLeft() > (uint)term[8]) )
	) ? false : true;
}
bool SaveData::validPageBattle(Array1D& term)
{
	int flags = term[1];

	return (
		( ( flags & (0x01 << 0) ) && !this->getFlag(term[2]) ) ||
		( ( flags & (0x01 << 1) ) && !this->getFlag(term[3]) ) ||
		( ( flags & (0x01 << 2) ) && (this->getVar(term[4]) < (int)term[5]) ) /* ||
	// turns
		( ( flags & (0x01 << 3) ) && ) ||
	// consume
		( ( flags & (0x01 << 4) ) && ) ||
	// enemy
		( ( flags & (0x01 << 5) ) && ) ||
	// party
		( ( flags & (0x01 << 6) ) && ) ||
 */
	) ? false : true;
}

uint SaveData::member(uint index)
{
	if( (index < 0) || (MEMBER.size() <= (uint)index) ) throw "Out of bound.";
	else return MEMBER[index];
}

EventState& SaveData::eventState(uint id)
{
	switch(id) {
		case EV_ID_PARTY: case EV_ID_BOAT: case EV_ID_SHIP: case EV_ID_AIRSHIP:
			 return (*this)[ 104 + (id-EV_ID_PARTY) ];
		case EV_ID_THIS: throw "Not supported.";
		default:
			return (EventState&) ( (Array2D&) ( (Array1D&) (*this)[111] )[11] )[id];
	}
}

void SaveData::replace(ChipSetType type, uint dstNo, uint srcNo)
{
	if( (CHIP_REPLACE_MAX <= dstNo) || (CHIP_REPLACE_MAX <= srcNo) ) {
		throw "Illegal map chip repalce.";
		return;
	}

	uint srcVal, dstVal;
	srcVal = CHIP_REPLACE[type][srcNo];
	dstVal = CHIP_REPLACE[type][dstNo];
	CHIP_REPLACE[type][dstNo] = srcVal;
	CHIP_REPLACE[type][srcNo] = dstVal;
}
void SaveData::resetReplace()
{
	for(uint i = 0; i < CHIP_TYPE_NUM; i++)
		for(uint j = 0; j < CHIP_REPLACE_MAX; j++) CHIP_REPLACE[i][j] = j;
}

	}; // namespace model
}; // namespace rpg2kLib
