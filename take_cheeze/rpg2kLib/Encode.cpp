#include "Debug.hpp"
#include "Encode.hpp"

using namespace rpg2kLib::debug;


namespace rpg2kLib
{
	namespace encode
	{

Encode::Encode()
{
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

std::string Encode::convertString(std::string src, iconv_t cd)
{
	char iconvBuff[BUFF_SIZE+1];
	size_t iconvOutSize = BUFF_SIZE, iconvInSize  = src.length() + 1;
	char *iconvOut = iconvBuff, *iconvIn  = const_cast<char*>( src.c_str() );

	size_t retValue = iconv(cd, &iconvIn, &iconvInSize, &iconvOut, &iconvOutSize);

	if(retValue == (size_t) -1) {
		int errnoBuf = errno;
		throw "iconv error: " + getError(errnoBuf);
	} else return std::string(iconvBuff); // , BUFF_SIZE-iconvOutSize);
}

bool Encode::isString(std::string src)
{
	unsigned char* ptr = (unsigned char*) src.c_str();
	for(uint i = 0; i < src.length(); i++, ptr++) {
		if( *ptr > 0x80 ) { ptr++; i++; }
		else if(*ptr < 0x20) return false;
	}
	return true;
}

	}; // namespace encode
}; // namespace rpg2kLib
