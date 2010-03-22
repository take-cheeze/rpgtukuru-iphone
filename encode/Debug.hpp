#ifndef _IN_RPG2K_DEBUG_DEBUG_HPP
#define _IN_RPG2K_DEBUG_DEBUG_HPP

#include <errno.h>

#include <iostream>
#include <fstream>
#include <stack>
#include <string>
#include <typeinfo>


namespace rpg2kLib
{
/*
	namespace structure
	{
		class Binary;
		class Element;
	};
 */

	namespace debug
	{

		extern std::string getError(int err);
		extern void addAtExitFunction( void (*func)(void) );
		// extern std::exception getException(const void* pthis);

		static const std::string COUT("out.txt"), CERR("err.txt");

		extern std::string demangleTypeInfo(const std::type_info& info);
#if RPG2K_USE_RTTI
		template< typename T >
		extern std::string demangle() { return demangleTypeInfo( typeid(T) ); }
		template< typename T >
		extern std::string demangle(T& src) { return demangleTypeInfo( typeid(src) ); }
#endif

		class Debug
		{
		private:
			static std::stack< std::streambuf* > COUT_FB_STACK, CERR_FB_STACK;

			static const int BUF_SIZE = 0;
			static char COUT_BUF[], CERR_BUF[];
		public:
			static void init();
			static void dispose();
		};

/*
		class Tracer
		{
		private:
		public:
			static void printTrace(structure::Element& e, bool info = false, std::ostream& ostrm = std::cerr);
			static void printInfo (structure::Element& b, std::ostream& ostrm = std::cerr);
		};
 */
	};
};

#endif
