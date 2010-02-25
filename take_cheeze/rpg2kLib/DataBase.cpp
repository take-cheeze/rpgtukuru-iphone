#include "DataBase.hpp"


namespace rpg2kLib
{
	namespace model
	{

DataBase::DataBase(std::string dir)
: Base(dir)
{
	init();
}
DataBase::DataBase(std::string dir, std::string name)
: Base(dir, name)
{
	init();
}
void DataBase::init()
{
	Base::load();
}

DataBase::~DataBase()
{
	std::cout << getHeader() << std::endl;
}

	}; // namespace model
}; // namespace rpg2kLib
