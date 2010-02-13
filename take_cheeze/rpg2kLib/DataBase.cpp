#include "DataBase.hpp"

using namespace rpg2kLib::model;

DataBase::DataBase(string dir, string name) : Base(dir, name)
{
	init();
}
void DataBase::init()
{
	Base::open();
}

DataBase::~DataBase()
{
	cout << getHeader() << endl;
}
