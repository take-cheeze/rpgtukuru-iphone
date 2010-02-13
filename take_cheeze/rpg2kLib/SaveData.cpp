#include <algorithm>

#include "SaveData.hpp"

using namespace rpg2kLib::model;

typedef map< uint16_t, SaveData::Item >::const_iterator item_it;

SaveData::SaveData(string dir, string name)
	: Base(dir, name), ID(0),
		ITEM(), VARIABLE(), SWITCH(), CHIP_REPLACE()
{
	init();
}
SaveData::SaveData(string dir, uint id)
	: Base(dir, ""), ID(id),
		ITEM(), VARIABLE(), SWITCH(), CHIP_REPLACE()
{
	char name[256];
	sprintf(name, "Save%02d.lsd", ID);
	setFileName(name);

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

	Base::open();

	Array1D& status = (*this)[109];
	Array1D& sys    = (*this)[101];

// item
	{
		int itemTypeNum = status[11];
		vector< uint16_t > id  = static_cast< Binary& >(status[12]);
		vector< uint8_t  > num = static_cast< Binary& >(status[13]);
		vector< uint8_t  > use = static_cast< Binary& >(status[14]);

		for(int i = 0; i < itemTypeNum; i++) {
			Item item = { num[i], use[i] };
			ITEM.insert( map< uint16_t, Item >::value_type(id[i], item) );
		}
	}
// switch and variable
	SWITCH.resize(sys[31]);
	SWITCH = static_cast< Binary& >(sys[32]);
	VARIABLE.resize(sys[33]);
	VARIABLE = static_cast< Binary& >(sys[34]);
// member
	MEMBER.resize(status[1]);
	MEMBER = static_cast< Binary& >(status[2]);
// chip replace
	CHIP_REPLACE.resize(CHIP_TYPE_NUM);
	for(uint i = 0; i < CHIP_TYPE_NUM; i++) {
		CHIP_REPLACE[i].resize(CHIP_REPLACE_MAX);
		CHIP_REPLACE[i] = static_cast< Binary& >( ( (Array1D&) (*this)[111] )[21+i] );
	}
}

SaveData::~SaveData()
{
	cout << getHeader() << endl;
}

void SaveData::save()
{
	Array1D& status = (*this)[109];
	Array1D& sys = (*this)[101];

// item
	{
		int itemNum = ITEM.size();
		status[11] = itemNum;

		vector< uint16_t > id (itemNum);
		vector< uint8_t  > num(itemNum);
		vector< uint8_t  > use(itemNum);

		int i = 0;
		for(item_it it = ITEM.begin(); it != ITEM.end(); ++it) {
			id [i] = it->first;
			num[i] = it->second.num;
			use[i] = it->second.use;

			i++;
		}
		static_cast< Binary& >(status[12]) = id ;
		static_cast< Binary& >(status[13]) = num;
		static_cast< Binary& >(status[14]) = use;
	}
// switch and variable
	sys[31] = SWITCH.size();
	static_cast< Binary& >(sys[32]) = SWITCH;
	sys[33] = VARIABLE.size();
	static_cast< Binary& >(sys[34]) = VARIABLE;
// member
	( (Array1D&)(*this)[109] )[1] = MEMBER.size();
	static_cast< Binary& >( ( (Array1D&)(*this)[109] )[2] ) = MEMBER;
// chip replace
	for(uint i = 0; i < CHIP_TYPE_NUM; i++)
		static_cast< Binary& >( ( (Array1D&) (*this)[111] )[21+i] ) = CHIP_REPLACE[i];

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
	return static_cast< Array1D& >( (*this)[109] )[21];
}
void SaveData::setMoney(int data)
{
	if(data < MONEY_MIN) data = MONEY_MIN;
	else if(MONEY_MAX < data) data = MONEY_MAX;

	static_cast< Array1D& >( (*this)[109] )[21] = data;
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
		ITEM.insert( map< uint, Item >::value_type(id, i) );
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
			vector< uint16_t > equip =
				static_cast< Binary& >( charParam(MEMBER[i])[61] );

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
		( ( flags & (0x01 << 2) ) && (this->getVar (term[4]) < term[5]) ) ||
		( ( flags & (0x01 << 3) ) && !this->hasItem(term[6]) ) ||
		(
			( flags & (0x01 << 4) ) &&
			( find( MEMBER.begin(), MEMBER.end(), (uint)term[7] ) == MEMBER.end() )
		) ||
		( ( flags & (0x01 << 5) ) && (this->timerLeft() > (uint)term[8]) )
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
