#include <cxxabi.h>

#include "Debug.hpp"
#include "Encode.hpp"

using namespace rpg2kLib::debug;
using namespace rpg2kLib::encode;

Encode::Encode()
{
	// TO_SYSTEM = iconv_open("hoge", "fuga");
	// clog << SYS_ENCODE << endl << TKOOL_ENCODE << endl;;

	TO_SYSTEM = iconv_open( SYS_ENCODE.c_str(), TKOOL_ENCODE.c_str() );
	if(TO_SYSTEM == (iconv_t)-1) {
		int errnoBuf = errno;
		throw "iconv_open error: " + getError(errnoBuf);

		TO_SYSTEM = NULL;
	}

	TO_TKOOL = iconv_open( TKOOL_ENCODE.c_str(), SYS_ENCODE.c_str() );
	if(TO_TKOOL  == (iconv_t)-1) {
		int errnoBuf = errno;
		throw "iconv_open error: " + getError(errnoBuf);

		TO_SYSTEM = NULL;
	}
}
Encode::~Encode()
{
	if( iconv_close(TO_SYSTEM) == -1 ) {
		int errnoBuf = errno;
		throw "iconv_close error: " + getError(errnoBuf);
	}
	if( iconv_close(TO_TKOOL ) == -1 ) {
		int errnoBuf = errno;
		throw "iconv_close error: " + getError(errnoBuf);
	}

	TO_SYSTEM = TO_TKOOL = NULL;
}

Encode& Encode::getInstance()
{
	static Encode theEncode;
	return theEncode;
}

string Encode::demangleTypeInfo(const type_info& info)
{
	int status;
	char* readable =  abi::__cxa_demangle( info.name(), NULL, NULL, &status );
// errors
	if(readable) throw "Demangling failed.";
	switch(status) {
		case -1: throw "Memory error.";
		case -2: throw "Invalid name.";
		case -3: throw "Argument was invalid";
		default: break;
	}
// char* to string
	string ret(readable);
	free(readable);

	return ret;
}

string Encode::toSystem(string src)
{
	char iconvBuff[BUFF_SIZE+1];

	size_t iconvOutSize = BUFF_SIZE, iconvInSize  = src.length() + 1;
	char *iconvOut = iconvBuff, *iconvIn  = const_cast<char*>( src.c_str() );

	size_t retValue =
		iconv(TO_SYSTEM, &iconvIn, &iconvInSize, &iconvOut, &iconvOutSize);

	if(retValue == (size_t) -1) {
		int errnoBuf = errno;
		throw "iconv error: " + getError(errnoBuf);
	} else return string(iconvBuff); // , BUFF_SIZE-iconvOutSize);
}
string Encode::toTkool (string src)
{
	char iconvBuff[BUFF_SIZE+1];
	size_t iconvOutSize = BUFF_SIZE, iconvInSize  = src.length() + 1;
	char *iconvOut = iconvBuff, *iconvIn  = const_cast<char*>( src.c_str() );

	size_t retValue =
		iconv(TO_TKOOL, &iconvIn, &iconvInSize, &iconvOut, &iconvOutSize);

	if(retValue == (size_t) -1) {
		int errnoBuf = errno;
		throw "iconv error: " + getError(errnoBuf);
	} else return string(iconvBuff); // , BUFF_SIZE-iconvOutSize);
}

bool Encode::isString(string src)
{
	unsigned char* ptr = (unsigned char*) src.c_str();
	for(uint i = 0; i < src.length(); i++, ptr++) {
		if( *ptr > 0x80 ) { ptr++; i++; }
		else if(*ptr < 0x20) return false;
	}
	return true;
}
