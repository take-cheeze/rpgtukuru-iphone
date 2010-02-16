#include "Element.hpp"

using namespace rpg2kLib::structure;

Array2D::Array2D(const Array2D& array)
	: BIN_DATA(),
		DATA(array.DATA), ARRAY_DEFINE(array.ARRAY_DEFINE),
		THIS(array.THIS) /* , EXISTS(array.EXISTS) */
{
}
Array2D::Array2D(Element& e, const Descriptor& info)
	: BIN_DATA(), DATA(), ARRAY_DEFINE( info.getArrayDefine() ), THIS(e)
{
	// EXISTS = false;
}
Array2D::Array2D(Element& e, const Descriptor& info, StreamReader& f)
	: BIN_DATA(), DATA(), ARRAY_DEFINE( info.getArrayDefine() ), THIS(e)
{
	init(f);
}
Array2D::Array2D(Element& e, const Descriptor& info, Binary& b)
	: BIN_DATA(), DATA(), ARRAY_DEFINE( info.getArrayDefine() ), THIS(e)
{
	StreamReader f = b;
	init(f);
}
void Array2D::init(StreamReader& s)
{
	for(uint i = 0, length = s.getBER(); i < length; i++) {
		uint index = s.getBER();
		DATA.add(index, *new Array1D(*this, index, s) );
	}

	if( /* isBinary(s) && */ !s.eof() ) throw "Didn't end with EOF.";
}
Array2D::~Array2D()
{
}

Array2D& Array2D::operator =(const Array2D& src)
{
	DATA = src.DATA;
	return *this;
}

Array1D& Array2D::operator [](uint index)
{
	if( DATA.exists(index) ) return DATA[index];
	else {
		DATA.add( index, *new Array1D(*this, index) );
		return DATA[index];
	}
}
const Array1D& Array2D::operator [](uint index) const
{
	if( DATA.exists(index) ) return DATA[index];
	else throw invalid_argument("Invalid index.");
}

uint Array2D::getSize()
{
	uint ret = 0, elementNum = 0;

	for(Iterator it = begin(); it != end(); ++it) {
	// index
		ret += getBERSize( it.first() );
	// data
		ret += it.second().getSize();

		elementNum++;
	}
	ret += getBERSize(elementNum);

	return ret;
}
const Binary& Array2D::toBinary()
{
	BIN_DATA.resize( getSize() );
	StreamWriter s(BIN_DATA);

	s << DATA.size();
	for(Iterator it = begin(); it != end(); ++it)
		s << it.first() << it.second().toBinary();

	return BIN_DATA;
}

/*
void Array2D::substantiate()
{
	// toElement().substantiate();

	// EXISTS = true;
}
 */

bool Array2D::exists(uint index) const
{
	return DATA.exists(index) ? DATA[index].exists() : false;
}
bool Array2D::exists(uint index1, uint index2) const
{
	return DATA.exists(index1) ? DATA[index1].exists(index2) : false;
}

template< > Map< uint, Array1D >::Iterator& Map< uint, Array1D >::Iterator::operator ++()
{
	do {
		if( (*this) != OWNER.end  () ) ++ITERATOR;
		else break;
	} while( !second().exists() );

	return (*this);
}
template< > Map< uint, Array1D >::Iterator& Map< uint, Array1D >::Iterator::operator --()
{
	do {
		if( (*this) != OWNER.begin() ) --ITERATOR;
		else break;
	} while( !second().exists() );

	return (*this);
}
template< > uint Map< uint, Array1D >::size() const
{
	uint ret = 0;
	for(Iterator it = begin(); it != end(); ++it) ret++;

	return ret;
}
template< > Map< uint, Array1D >::Iterator Map< uint, Array1D >::begin() const
{
	for( Iterator it = Map< uint, Array1D >::begin(); it != Map< uint, Array1D >::end(); ++it) {
		if( it.second().exists() ) return it;
	}

	return Map< uint, Array1D >::begin();
}
template< > Map< uint, Array1D >::Iterator Map< uint, Array1D >::end  () const
{
	for( Iterator it = Map< uint, Array1D >::end(); it != Map< uint, Array1D >::begin(); --it) {
		if( it.second().exists() ) break;
	}

	return Map< uint, Array1D >::end();
}
