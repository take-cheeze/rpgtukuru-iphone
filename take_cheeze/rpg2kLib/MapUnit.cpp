#include "MapUnit.hpp"

#include <sstream>

using namespace rpg2kLib::model;

MapUnit::MapUnit(string dir, string name) : Base(dir, name), ID(0)
{
	init();
}
MapUnit::MapUnit(string dir, uint id) : Base(dir, ""), ID(id)
{
	string name;
	ostringstream strm(name);
	strm << "Map";
	strm.fill('0'); strm.width(4); strm << id;
	strm << ".lmu";
	setFileName(name);

	checkExists();

	init();
}
void MapUnit::init()
{
	if( (ID < 1) || (MAP_UNIT_MAX < ID) ) throw invalid_argument("mapID");

	Base::load();

	LOWER = static_cast< Binary& >( (*this)[71] );
	UPPER = static_cast< Binary& >( (*this)[72] );

	WIDTH  = (*this)[2];
	HEIGHT = (*this)[3];
}

MapUnit::~MapUnit()
{
	cout << getHeader() << endl;
}

void MapUnit::save()
{
	static_cast< Binary& >( (*this)[71] ) = LOWER;
	static_cast< Binary& >( (*this)[72] ) = UPPER;

	(*this)[2] = WIDTH ;
	(*this)[3] = HEIGHT;
}

int MapUnit::chipIDLw(uint x, uint y) const
{
	if( ( x >= getWidth() ) || ( y >= getHeight() ) ) throw "x or y is too big";
	else return LOWER[getWidth()*y + x];
}
int MapUnit::chipIDUp(uint x, uint y) const
{
	if( ( x >= getWidth() ) || ( y >= getHeight() ) ) throw "x or y is too big";
	else return UPPER[getWidth()*y + x];
}
