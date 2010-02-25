#ifndef _INC__RGP2K__ENCODE__ENCODE_HPP
#define _INC__RGP2K__ENCODE__ENCODE_HPP

#include "Defines.hpp"

#include <string>
#include <typeinfo>

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

			static std::string demangleTypeInfo(const std::type_info& info);
			static std::string convertString(std::string src, iconv_t cd);
		public:
			static Encode& getInstance();

			std::string toSystem(std::string src) { return convertString(src, TO_SYSTEM); }
			std::string toTkool (std::string src) { return convertString(src, TO_TKOOL ); }

			template< typename T >
			static std::string demangle() { return demangleTypeInfo( typeid(T) ); }

			template< typename T >
			static std::string demangle(T& src) { return demangleTypeInfo( typeid(src) ); }

			bool isString(std::string src);
		};

	};
};

#endif
