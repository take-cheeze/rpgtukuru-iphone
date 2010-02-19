#include "MapTree.hpp"

using namespace rpg2kLib::model;

MapTree::MapTree(string dir) : Base(dir)
{
	init();
}
MapTree::MapTree(string dir, string name) : Base(dir, name)
{
	init();
}
void MapTree::init()
{
	Base::load();
/*
	BerEnum& existSrc = getExist();
	for(uint i = 0; i < existSrc.size(); i++) {
		EXISTS.insert( map< uint, bool >::value_type( existSrc[i], true ) );
	}
 */
}

MapTree::~MapTree()
{
	cout << getHeader() << endl;
}

/*
bool MapTree::exists(uint mapID)
{
	if( EXISTS.find(mapID) != EXISTS.end() ) return EXISTS[mapID];
	else return false;
}
 */

bool MapTree::canTeleport(uint mapID) const
{
	switch( static_cast< int32_t >( (*this)[mapID][31] ) )
	{
		case 0: return canTeleport( (*this)[mapID][2] );
		case 1: return true ;
		case 2: return false;
		default: throw range_error("Cannot get teleport restriction.");
	}
}
bool MapTree::canEscape(uint mapID) const
{
	switch( static_cast< int32_t >( (*this)[mapID][32] ) )
	{
		case 0: return canTeleport( (*this)[mapID][2] );
		case 1: return true ;
		case 2: return false;
		default: throw range_error("Cannot get teleport restriction.");
	}
}
bool MapTree::canSave(uint mapID) const
{
	switch( static_cast< int32_t >( (*this)[mapID][33] ) )
	{
		case 0: return canTeleport( (*this)[mapID][2] );
		case 1: return true ;
		case 2: return false;
		default: throw range_error("Cannot get teleport restriction.");
	}
}
