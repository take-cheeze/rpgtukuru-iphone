#include "DataBase.hpp"

using namespace rpg2kLib::model;

DataBase::DataBase(string dir) : Base(dir)
{
	init();
}
DataBase::DataBase(string dir, string name) : Base(dir, name)
{
	init();
}
void DataBase::init()
{
	Base::load();
}

DataBase::~DataBase()
{
	cout << getHeader() << endl;
}
