#include <stdexcept>

#include "Debug.hpp"
#include "Encode.hpp"

#define getlang() getenv("LANG")


namespace rpg2k
{
	#if RPG2K_IS_WINDOWS
		std::string Encode::SYSTEM_ENCODE = "Windows-31J";
	#else
		std::string Encode::SYSTEM_ENCODE = "UTF-8";
	#endif
	std::string const Encode::RPG2K_ENCODE = "Windows-31J"; // "Shift_JIS";

	Encode::Encode()
	{
		std::string sysEncode = SYSTEM_ENCODE;

		/*
		 * geting system encoding name from "LANG" env
		 * works only on unix systems(as I know)
		 */
		if( getlang() ) {
			std::string langStr = getlang();
			uint pos = langStr.find('.');
			if( pos != std::string::npos ) {
				sysEncode = langStr.substr( pos + 1 );
				// clog << sysEncode << endl;
			}
		}

		toSystem_ = openConverter(sysEncode, RPG2K_ENCODE);
		toRPG2k_  = openConverter(RPG2K_ENCODE, sysEncode);
	}
	Encode::~Encode()
	{
		rpg2k_assert( ::iconv_close(toSystem_) != -1 );
		rpg2k_assert( ::iconv_close(toRPG2k_ ) != -1 );

		toSystem_ = toRPG2k_ = NULL;
	}

	Encode& Encode::instance()
	{
		static Encode theEncode;
		return theEncode;
	}

	iconv_t Encode::openConverter(std::string const& to, std::string const& from)
	{
		iconv_t ret = ::iconv_open( to.c_str(), from.c_str() );
		rpg2k_assert( ret != (iconv_t)-1 );
		return ret;
	}

	std::string Encode::convertString(std::string const& src, iconv_t cd)
	{
		char iconvBuff[BUFF_SIZE];
		size_t iconvOutSize = BUFF_SIZE, iconvInSize  = src.length();
		char* iconvOut = iconvBuff;
		#if RPG2K_IS_PSP && !defined(_LIBICONV_H)
			char const* iconvIn  = src.c_str();
		#else
			char* iconvIn  = const_cast< char* >( src.c_str() );
		#endif

		rpg2k_assert( ::iconv(cd, &iconvIn, &iconvInSize, &iconvOut, &iconvOutSize) != (size_t) -1 );
		return std::string(iconvBuff, BUFF_SIZE-iconvOutSize);
	}
} // namespace rpg2k
