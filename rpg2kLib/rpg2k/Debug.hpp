#ifndef _INC_RPG2K__DEBUG__DEBUG_HPP
#define _INC_RPG2K__DEBUG__DEBUG_HPP

#include "Define.hpp"

#include <errno.h>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <stack>
#include <string>
#include <typeinfo>


namespace rpg2k
{
	using std::cerr;
	using std::clog;
	using std::cout;
	using std::endl;

	class Binary;
	namespace structure
	{
		class Element;
		class Event;
	} // namespace structure

	namespace debug
	{
		extern std::string getError(int err);
		extern void addAtExitFunction( void (*func)(void) );

		extern std::string demangleTypeInfo(std::type_info const& info);
		#if RPG2K_USE_RTTI
			template< typename T >
			inline std::string demangle() { return demangleTypeInfo( typeid(T) ); }
			template< typename T >
			inline std::string demangle(T& src) { return demangleTypeInfo( typeid(src) ); }
		#endif

		class Logger
		{
		private:
			static SystemString const COUT_NAME, CERR_NAME;
			static std::stack< std::streambuf* > COUT_FB_STACK, CERR_FB_STACK;

			static int const BUF_SIZE = 0;
			static char COUT_BUF[], CERR_BUF[];
		protected:
			static void dispose();
		public:
			static void init();
		}; // class Logger

		extern std::ofstream ANALYZE_RESULT;

		class Tracer
		{
		public:
			#define PP_defaultOutput std::ostream& ostrm = ANALYZE_RESULT
			static void printTrace(structure::Element& e, bool info = false, PP_defaultOutput);
			static void printInfo (structure::Element& e, PP_defaultOutput);

			static void printInt   (int    val, PP_defaultOutput);
			static void printBool  (bool   val, PP_defaultOutput);
			static void printDouble(double val, PP_defaultOutput);
			static void printString(RPG2kString const& val, PP_defaultOutput);
			static void printEvent (structure::Event  const& val, PP_defaultOutput);
			static void printBinary(Binary const& val, PP_defaultOutput);
			#undef PP_defaultOutput
		}; // class Tracer
	} // namespace debug
} // namespace rpg2k

#endif
