#include <stdexcept>

#include "Debug.hpp"
#include "Encode.hpp"

#define getlang() getenv("LANG")


namespace rpg2k
{
	#if RPG2K_IS_WINDOWS
		std::string const Encode::SYSTEM_ENCODE = "Windows-31J";
	#else
		std::string const Encode::SYSTEM_ENCODE = "UTF-8";
	#endif
	std::string const Encode::RPG2K_ENCODE = "Windows-31J"; // "Shift_JIS";

	Encode::Encode()
	{
		sysEncode_ = SYSTEM_ENCODE;

		/*
		 * geting system encoding name from "LANG" env
		 * works only on unix systems(as I know)
		 */
		if( getlang() ) {
			std::string const langStr = getlang();
			std::size_t pos = langStr.find('.');
			if( pos != std::string::npos ) {
				sysEncode_ = langStr.substr( pos + 1 );
				// clog << sysEncode_ << endl;
			}
		}

		toSystem_ = openConverter(sysEncode_, RPG2K_ENCODE);
		toRPG2k_  = openConverter(RPG2K_ENCODE, sysEncode_);
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

		std::string ret;
		while(iconvInSize) {
			if( ::iconv(cd, &iconvIn, &iconvInSize, &iconvOut, &iconvOutSize) == (size_t) -1 ) {
				throw std::runtime_error("char encoding convert error");
			}
			ret.append(iconvBuff, BUFF_SIZE-iconvOutSize);
			iconvOutSize = BUFF_SIZE;
		}
		return ret;
	}
} // namespace rpg2k
