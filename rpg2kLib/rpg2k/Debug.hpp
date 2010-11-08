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
#include <exception>


namespace rpg2k
{
	using std::cerr;
	using std::clog;
	using std::cout;
	using std::endl;

	class Binary;
	namespace structure
	{
		class Array1D;
		class Array2D;
		class Element;
		class Event;
		class Instruction;
	} // namespace structure

	namespace debug
	{

		std::string error(int errNo);
		void addAtExitFunction( void (*func)(void) );

		std::string demangleTypeInfo(std::type_info const& info);
		#if RPG2K_USE_RTTI
			template<typename T>
			inline std::string demangle() { return demangleTypeInfo( typeid(T) ); }
			template<typename T>
			inline std::string demangle(T& src) { return demangleTypeInfo( typeid(src) ); }
		#endif

		extern std::ofstream ANALYZE_RESULT; // usually analyze.txt

		class AnalyzeException : public std::exception {};
		#if RPG2K_DEBUG
			#define rpg2k_analyze_assert(EXP) if( !(EXP) ) throw debug::AnalyzeException()
		#else
			#define rpg2k_analyze_assert(EXP) rpg2k_assert(EXP)
		#endif

		struct Tracer
		{
			#define PP_defaultOutput std::ostream& ostrm = ANALYZE_RESULT
			static std::ostream& printTrace(structure::Element const& e, bool info = false, PP_defaultOutput);
			static std::ostream& printInfo (structure::Element const& e, PP_defaultOutput);

			static std::ostream& printInt   (int    val, PP_defaultOutput);
			static std::ostream& printBool  (bool   val, PP_defaultOutput);
			static std::ostream& printDouble(double val, PP_defaultOutput);
			static std::ostream& printString(RPG2kString const& val, PP_defaultOutput);
			static std::ostream& printEvent (structure::Event const& val, PP_defaultOutput);
			static std::ostream& printInstruction(structure::Instruction const& inst, PP_defaultOutput, bool indent = false);
			static std::ostream& printBinary(Binary const& val, PP_defaultOutput);
			static std::ostream& printArray1D(structure::Array1D const& val, PP_defaultOutput);
			static std::ostream& printArray2D(structure::Array2D const& val, PP_defaultOutput);
			#undef PP_defaultOutput
		}; // class Tracer
	} // namespace debug
} // namespace rpg2k

#endif
