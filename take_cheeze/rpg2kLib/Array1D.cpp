#include "Debug.hpp"
#include "Element.hpp"

using namespace rpg2kLib::debug;
using namespace rpg2kLib::structure;

Array1D::Array1D(const Array1D& array)
	: DATA(array.DATA), ARRAY_DEFINE(array.ARRAY_DEFINE), THIS(array.THIS),
		EXISTS(array.EXISTS), OWNER(array.OWNER), INDEX(array.INDEX)
{
}
Array1D::Array1D(Element& e, const Descriptor& info)
	: ARRAY_DEFINE( info.getArrayDefine() ), THIS(e)
{
	OWNER = NULL;
	EXISTS = false;
}
Array1D::Array1D(Element& e, const Descriptor& info, Stream& f)
	: ARRAY_DEFINE( info.getArrayDefine() ), THIS(e)
{
	init(f);
}
Array1D::Array1D(Element& e, const Descriptor& info, Binary& b)
	: ARRAY_DEFINE( info.getArrayDefine() ), THIS(e)
{
	Stream s(b);
	init(s);
}
Array1D::Array1D(Array2D& owner, uint index)
	: ARRAY_DEFINE( owner.getArrayDefine() ), THIS( owner.toElement() )
{
	EXISTS = false;
	INDEX = index;
	OWNER = &owner;
}
Array1D::Array1D(Array2D& owner, uint index, Stream& s)
	: ARRAY_DEFINE( owner.getArrayDefine() ), THIS( owner.toElement() )
{
	EXISTS = true;
	INDEX = index;
	OWNER = &owner;

	// bool file = isFile(s);
	Binary b;

	while(true) {
		uint index2 = s.getBER();

		if(index2 == ARRAY_1D_END) break;
		else DATA.add( index2, *new Element( owner, index, index2, s.get(b) ) );

		// if( file && s.eof() ) return;
	}
}
void Array1D::init(Stream& s)
{
	OWNER = NULL;
	EXISTS = true;

	bool file = isFile(s);
	Binary b;

	while(true) {
		uint index = s.getBER();

		if(index == ARRAY_1D_END) break;
		else DATA.add( index, *new Element( *this, index, s.get(b) ) );

		if( file && s.eof() ) return;
	}

	if( !s.eof() ) throw "Didn't end with EOF.";
}

Array1D::~Array1D()
{
}

Array1D& Array1D::operator =(const Array1D& src)
{
	EXISTS = src.EXISTS;
	OWNER = src.OWNER;
	INDEX = src.INDEX;

	DATA = src.DATA;
	return *this;
}

// #define TRACE_ALL

Element& Array1D::operator [](uint index)
{
#if defined TRACE_ALL
	if( isArray2D() ) {
		Tracer::printTrace( toElement(), false, clog );
		clog << "Array2D[";
		clog.width(6); clog << dec << getIndex() << "][";
		clog.width(6); clog << dec << index << "];" << endl;
	} else {
		Tracer::printTrace( toElement(), false, clog );
		clog << "Array1D[";
		clog.width(6); clog << dec << index << "];" << endl;
	}
#endif

	if( DATA.exists(index) ) return DATA[index];
	else {
		DATA.add( index, *new Element(*this, index) );
		return DATA[index];
	}
}
const Element& Array1D::operator [](uint index) const
{
#if defined TRACE_ALL
	if( isArray2D() ) {
		Tracer::printTrace( toElement(), false, clog );
		clog << "Array2D[";
		clog.width(6); clog << dec << getIndex() << "][";
		clog.width(6); clog << dec << index << "];" << endl;
	} else {
		Tracer::printTrace( toElement(), false, clog );
		clog << "Array1D[";
		clog.width(6); clog << dec << index << "];" << endl;
	}
#endif

	if( DATA.exists(index) ) return DATA[index];
	else throw invalid_argument("Invalid index.");
}

uint Array1D::getSize()
{
	uint ret = 0;

	for(Iterator it = begin(); it != end(); ++it) {
	// index
		ret += getBERSize( it.first() );
	// data
		Binary& data = it.second();
		ret += getBERSize(data.length());
		ret += data.length();
	}
	ret += getBERSize(ARRAY_1D_END);

	return ret;
}
const Binary& Array1D::toBinary()
{
	BIN_DATA.reset( getSize() );
	Stream f(BIN_DATA);

	for(Iterator it = begin(); it != end(); ++it) f << it.first() <= it.second();
	f << ARRAY_1D_END;

	return BIN_DATA;
}

uint Array1D::getIndex() const
{
	if( isArray2D() ) return INDEX;
	else throw "Doesn't have owner.";
}

bool Array1D::exists() const
{
	if( isArray2D() ) return EXISTS;
	else throw "Doesn't have owner.";
}

Array2D& Array1D::getOwner() const
{
	if( isArray2D() ) return *OWNER;
	else throw "Doesn't have owner.";
}

void Array1D::substantiate()
{
	if( isArray2D() ) {
		toElement().substantiate();
		EXISTS = true;
	} else throw "Doesn't have owner.";
}
bool Array1D::exists(uint index) const
{
	return DATA.exists(index) ? DATA[index].exists() : false;
}

template< > Map< uint, Element >::Iterator& Map< uint, Element >::Iterator::operator ++()
{
	do {
		if( (*this) != OWNER.end  () ) ++ITERATOR;
		else break;
	} while( !second().exists() );

	return (*this);
}
template< > Map< uint, Element >::Iterator& Map< uint, Element >::Iterator::operator --()
{
	do {
		if( (*this) != OWNER.begin() ) --ITERATOR;
		else break;
	} while( !second().exists() );

	return (*this);
}
template< > uint Map< uint, Element >::size() const
{
	uint ret = 0;
	for(Iterator it = begin(); it != end(); ++it) ret++;

	return ret;
}
template< > Map< uint, Element >::Iterator Map< uint, Element >::begin() const
{
	for( Iterator it = Map< uint, Element >::begin(); it != Map< uint, Element >::end(); ++it) {
		if( it.second().exists() ) return it;
	}

	return Map< uint, Element >::begin();
}
template< > Map< uint, Element >::Iterator Map< uint, Element >::end  () const
{
	for( Iterator it = Map< uint, Element >::end(); it != Map< uint, Element >::begin(); --it) {
		if( it.second().exists() ) break;
	}

	return Map< uint, Element >::end();
}
