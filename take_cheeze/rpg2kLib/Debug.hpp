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
		using namespace std;
		using namespace rpg2kLib::structure;

		extern string getError(int err);
		extern void addAtExitFunction( void (*func)(void) );

		static const string COUT("out.txt"), CERR("err.txt");

		class Debug
		{
		private:
			static stack< streambuf* > COUT_FB_STACK, CERR_FB_STACK;

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
			static void printTrace(Element& e, bool info = false, ostream& ostrm = cerr);
			static void printInfo(Element& b, ostream& ostrm = cerr);
		};
	};
};

#endif
