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

		using namespace std;

		class Encode
		{
		private:
			static const int BUFF_SIZE = 1024;

			iconv_t TO_SYSTEM, TO_TKOOL;
		protected:
			Encode();
			Encode(const Encode& e);

			~Encode();

			string demangleTypeInfo(const type_info& info);
		public:
			static Encode& getInstance();

			string toSystem(string src);
			string toTkool (string src);

			template< typename T >
			string demangle() { return demangleTypeInfo( typeid(T) ); }

			template< typename T >
			string demangle(T& src) { return demangleTypeInfo( typeid(src) ); }

			bool isString(string src);
		};

	};
};

#endif
