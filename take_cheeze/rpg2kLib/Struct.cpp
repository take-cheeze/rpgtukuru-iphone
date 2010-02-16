#include "Element.hpp"

using namespace rpg2kLib::structure;

uint rpg2kLib::structure::getBERSize(uint32_t num)
{
	if(!num) return 1;
	else {
		uint ret = 0;
		for(; num; num >>= BER_BIT) ret++;
		return ret;
	}
}

Binary::Binary(Element& e, const Descriptor& info)
{
}
Binary::Binary(Element& e, const Descriptor& info, Binary& b)
{
	*this = b;
}
Binary::Binary(Element& e, const Descriptor& info, StreamReader& s)
{
	throw "Not supported.";
}

bool Binary::isNumber() const
{
	Binary b = *this;
	StreamReader s(b);

	try {
		s.getBER();
	} catch(...) { return false; }

	return s.eof();
}

int Binary::toNumber() const
{
	Binary b = *this;
	StreamReader f(b);

	int ret = f.getBER();
	if(getBERSize(ret) == length()) return ret;
	else {
		cerr << "Binary::getNumber(): This Binary isn't BER number. "
			<< "length() = " << dec << length()
			<< "; getBERSize(" << ret << ") = " << getBERSize(ret)
			<< ";" << endl;
		throw "Failed converting  from Binary to number.";
	}
}
bool Binary::toBool() const
{
	if( length() != sizeof(bool) )
		cerr << "Binary::getBool(): This Binary isn't bool. "
			<< "length() = " << length() << ";" << endl;
	else if(toNumber() == 0) return false;
	else if(toNumber() == 1) return true;

	throw "Failed converting  from Binary to bool.";
}
double Binary::toDouble() const
{
	if( length() != sizeof(double) ) {
		cerr << "Binary::getDouble(): This Binary isn't double. "
			<< "length() = " << length() << ";" << endl;
		throw "Failed converting  from Binary to double.";
	} else return *( (double*)getPtr() );
}

void Binary::setString(string str)
{
	resize( str.length() );
	memcpy( this->getPtr(), str.c_str(), length() );
}
void Binary::setNumber(int num)
{
	resize( getBERSize(num) );
	StreamWriter(*this).setBER(num);
}
void Binary::setBool(bool b)
{
	resize( sizeof(bool) );
	*( this->getPtr() ) = b;
}
void Binary::setDouble(double d)
{
	resize( sizeof(double) );
	*( (double*)this->getPtr() ) = d;
}
