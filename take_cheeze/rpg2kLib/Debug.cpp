#include <cstdlib>

#include <stack>
#include <list>

#include "Debug.hpp"
#include "Encode.hpp"
#include "Element.hpp"

using namespace rpg2kLib::encode;
using rpg2kLib::structure::Binary ;
using rpg2kLib::structure::Element;


namespace rpg2kLib
{
	namespace debug
	{

std::string getError(int errNum)
{
	const char* message = strerror(errNum);
	switch(errno) {
		case EINVAL: throw "Invalid Error Descriptor.";
		default: return message;
	}
}
void addAtExitFunction( void (*func)(void) )
{
	if( atexit(func) != 0 ) throw "atexit() error.";
}

char Debug::COUT_BUF[BUF_SIZE], Debug::CERR_BUF[BUF_SIZE];
std::stack< std::streambuf* > Debug::COUT_FB_STACK, Debug::CERR_FB_STACK;

void Debug::init() {
	std::filebuf* fb = new std::filebuf();

	fb->open(COUT.c_str(), std::ios::out);
	fb->pubsetbuf(COUT_BUF, BUF_SIZE);
	COUT_FB_STACK.push( std::cout.rdbuf(fb) );

	fb = new std::filebuf();
	fb->open(CERR.c_str(), std::ios::out);
	fb->pubsetbuf(CERR_BUF, BUF_SIZE);
	CERR_FB_STACK.push( std::cerr.rdbuf(fb) );

	std::cout << "Debug start at ";
	std::cerr << "Debug start at ";
#if defined(PSP)
	std::cout << "PSP." << std::endl << std::endl;
	std::cerr << "PSP." << std::endl << std::endl;
#else
	std::cout << "PC." << std::endl << std::endl;
	std::cerr << "PC." << std::endl << std::endl;
#endif
}
void Debug::dispose() {
	if( !COUT_FB_STACK.empty() && !CERR_FB_STACK.empty() ) {
		delete (std::filebuf*) std::cout.rdbuf( COUT_FB_STACK.top() );
		delete (std::filebuf*) std::cerr.rdbuf( CERR_FB_STACK.top() );

		COUT_FB_STACK.pop();
		CERR_FB_STACK.pop();
	}
}

void Tracer::printTrace(Element& e, bool info, std::ostream& ostrm)
{
	std::stack< Element*, std::list< Element* > > st;

	st.push(&e);
	for( Element* buf = st.top(); buf->hasOwner(); buf = st.top() ) {
		// std::clog << hex << st.top() << std::endl;
		st.push( &buf->getOwner() );
	}

	while( !st.empty() ) {
		Element& cur = *( st.top() ); st.pop();

		if( cur.hasOwner() ) {
			ostrm << cur.getOwner().getDescriptor().getTypeName() << "[";
			/* ostrm.width(6); */ ostrm << std::dec << cur.getIndex1() << "]";
			try {
				uint index2 = cur.getIndex2();
				ostrm << "["; /* ostrm.width(6); */ ostrm << std::dec << index2 << "]";
			} catch(...) {}
			ostrm << ": ";
		}
	}

	if(info) Tracer::printInfo(e, ostrm);
}

void Tracer::printInfo(Element& e, std::ostream& ostrm)
{
	if( e.isUndefined() ) {
		ostrm << "\n";
		const Binary& b = e.toBinary();
	// check whether it's empty
		if( b.length() == 0 ) {
			ostrm << "\t" "This data is empty.\n";
			return;
		} else {
		// Binary
			ostrm << "\t" "Binary: size = ";
			ostrm.width(8); ostrm << b.length() << "; data = { ";

			ostrm.fill('0');
			for(uint i = 0; i < b.length(); i++) {
				ostrm.width(2); ostrm << std::hex << (b[i] & 0xff) << " ";
			}
			ostrm.fill(' ');

			ostrm << "};\n";
		// BER number
			if( b.isNumber() ) {
				ostrm << "\t" "BER: ";
				ostrm.width(8); ostrm << std::dec << (int) b.toNumber() << ";\n";
			}
		// string
			if( Encode::getInstance().isString(b) ) {
				try {
					std::string str = Encode::getInstance().toSystem(b);
					ostrm << "\t" "string: " << str << ";\n";
				} catch(...) {}
			}
		}
	} else {
		std::string type = e.getDescriptor().getTypeName();
		if(type == "Binary") {
			ostrm << "\n";
			Binary& b = e;

			ostrm << "\t" << type << ": size = ";
			ostrm.width(8); ostrm << b.length() << "; data = { ";

			ostrm.fill('0');
			for(uint i = 0; i < b.length(); i++) {
				ostrm.width(2); ostrm << std::hex << (b[i] & 0xff) << " ";
			}
			ostrm.fill(' ');

			ostrm << "}";
		} else if(type == "bool") {
			if( (bool)e ) ostrm << "true";
			else ostrm << "false";
		} else if(type == "double") {
			ostrm << (double)e;
		} else if(type == "string") {
			try {
				ostrm << "\"" << Encode::getInstance().toSystem(e) << "\"";
			} catch(...) {
				ostrm << "\"\"";
			}
		} else if(type == "int") {
			ostrm << std::dec << (int)e;
		} else ostrm << type;

		ostrm << ";" << std::endl;
	}
}

	}; // namespace debug
}; // namespace rpg2kLib
