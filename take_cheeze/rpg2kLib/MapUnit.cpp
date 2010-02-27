#include "MapUnit.hpp"

#include <iomanip>
#include <sstream>


namespace rpg2kLib
{
	namespace model
	{

MapUnit::MapUnit(std::string dir, std::string name)
: Base(dir, name), ID(0)
{
	init();
}
MapUnit::MapUnit(std::string dir, uint id)
: Base(dir, ""), ID(id)
{
	std::ostringstream ss("Map");
	ss << std::setfill('0') << std::setw(4) << id << ".lmu";
	setFileName( ss.str() );

	checkExists();

	init();
}
void MapUnit::init()
{
	if( (ID < 1) || (MAP_UNIT_MAX < ID) ) throw std::invalid_argument("mapID");

	Base::load();

	LOWER = (*this)[71].getBinary();
	UPPER = (*this)[72].getBinary();

	WIDTH  = (*this)[2];
	HEIGHT = (*this)[3];
}

MapUnit::~MapUnit()
{
	std::cout << getHeader() << std::endl;
}

void MapUnit::save()
{
	(*this)[71].getBinary() = LOWER;
	(*this)[72].getBinary() = UPPER;

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

	}; // namespace model
}; // namespace rpg2kLib 
