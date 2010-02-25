#ifndef _IN_RPG2K_DEBUG_DEBUG_HPP
#define _IN_RPG2K_DEBUG_DEBUG_HPP

#include <errno.h>

#include <iostream>
#include <fstream>
#include <stack>
#include <string>

namespace rpg2kLib
{
	namespace structure
	{
		class Binary;
		class Element;
	};

	namespace debug
	{

		extern std::string getError(int err);
		extern void addAtExitFunction( void (*func)(void) );

		static const std::string COUT("out.txt"), CERR("err.txt");

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

		class Tracer
		{
		private:
		public:
			static void printTrace(structure::Element& e, bool info = false, std::ostream& ostrm = std::cerr);
			static void printInfo (structure::Element& b, std::ostream& ostrm = std::cerr);
		};
	};
};

#endif
