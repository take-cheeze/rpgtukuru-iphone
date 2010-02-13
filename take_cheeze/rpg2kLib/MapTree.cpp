#include "MapTree.hpp"

using namespace rpg2kLib::model;

MapTree::MapTree(string dir, string name) : Base(dir, name)
{
	init();
}
void MapTree::init()
{
	Base::open();

	BerEnum& existSrc = getExist();
	for(uint i = 0; i < existSrc.size(); i++) {
		EXISTS.insert( map< uint, bool >::value_type( existSrc[i], true ) );
	}
}

MapTree::~MapTree()
{
	cout << getHeader() << endl;
}

bool MapTree::exists(uint mapID)
{
	if( EXISTS.find(mapID) != EXISTS.end() ) return EXISTS[mapID];
	else return false;
}
