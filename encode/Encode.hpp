#ifndef _INC__RGP2K__ENCODE__ENCODE_HPP
#define _INC__RGP2K__ENCODE__ENCODE_HPP

#include "Defines.hpp"

#include <iconv.h>

namespace rpg2kLib
{
	namespace encode
	{

		class Encode
		{
		private:
			static const int BUFF_SIZE = 1024;

			iconv_t TO_SYSTEM, TO_TKOOL;
		protected:
			Encode();
			Encode(const Encode& e);

			~Encode();

			static std::string convertString(const std::string& src, iconv_t cd);
		public:
			static Encode& getInstance();

			std::string toSystem(const std::string& src) { return convertString(src, TO_SYSTEM); }
			std::string toTkool (const std::string& src) { return convertString(src, TO_TKOOL ); }

			bool isString(const std::string& src);
		};
	
	}	// namespace encode
}	// namespace rpg2kLib

#endif	// _INC__RGP2K__ENCODE__ENCODE_HPP

