#include <stdexcept>

#include "Debug.hpp"
#include "Encode.hpp"


namespace rpg2kLib
{
	namespace encode
	{

Encode::Encode()
{
	TO_SYSTEM = iconv_open( SYS_ENCODE.c_str(), TKOOL_ENCODE.c_str() );
	if( TO_SYSTEM == (iconv_t)-1 ) {
		int errnoBuf = errno;
		throw std::runtime_error( "iconv_open error: " + debug::getError(errnoBuf) );
		TO_SYSTEM = NULL;
	}

	TO_TKOOL = iconv_open( TKOOL_ENCODE.c_str(), SYS_ENCODE.c_str() );
	if( TO_TKOOL  == (iconv_t)-1 ) {
		int errnoBuf = errno;
		throw std::runtime_error( "iconv_open error: " + debug::getError(errnoBuf) );
		TO_TKOOL  = NULL;
	}
}
Encode::~Encode()
{
	if( iconv_close(TO_SYSTEM) == -1 ) {
		int errnoBuf = errno;
		throw std::runtime_error( "iconv_close error: " + debug::getError(errnoBuf) );
	}
	if( iconv_close(TO_TKOOL ) == -1 ) {
		int errnoBuf = errno;
		throw std::runtime_error( "iconv_close error: " + debug::getError(errnoBuf) );
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
	char* iconvOut = iconvBuff;
#if defined(RPG2K_IS_PSP)
	const char* iconvIn  = src.c_str();
#else
	char* iconvIn  = const_cast<char*>( src.c_str() );
#endif

	size_t retValue = iconv(cd, &iconvIn, &iconvInSize, &iconvOut, &iconvOutSize);

	if(retValue == (size_t) -1) {
		int errnoBuf = errno;
		throw std::runtime_error( "iconv error: " + debug::getError(errnoBuf) );
	} else return std::string(iconvBuff);
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
