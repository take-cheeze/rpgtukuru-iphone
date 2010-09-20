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
		class Instruction;
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

		extern std::ofstream ANALYZE_RESULT; // usually analyze.txt

		class Tracer
		{
		public:
			#define PP_defaultOutput() std::ostream& ostrm = ANALYZE_RESULT
			static std::ostream& printTrace(structure::Element& e, bool info = false, PP_defaultOutput());
			static std::ostream& printInfo (structure::Element& e, PP_defaultOutput());

			static std::ostream& printInt   (int    val, PP_defaultOutput());
			static std::ostream& printBool  (bool   val, PP_defaultOutput());
			static std::ostream& printDouble(double val, PP_defaultOutput());
			static std::ostream& printString(RPG2kString const& val, PP_defaultOutput());
			static std::ostream& printEvent (structure::Event const& val, PP_defaultOutput());
			static std::ostream& printInstruction(structure::Instruction const& inst, PP_defaultOutput(), bool indent = false);
			static std::ostream& printBinary(Binary const& val, PP_defaultOutput());
			#undef PP_defaultOutput
		}; // class Tracer
	} // namespace debug
} // namespace rpg2k

#endif
