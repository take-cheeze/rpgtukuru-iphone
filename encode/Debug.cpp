#include <cstdlib>
#include <cstring>
#include <cxxabi.h>

#include <iomanip>
#include <list>
#include <stdexcept>

#include "Debug.hpp"
#include "Encode.hpp"
// #include "Element.hpp"

/*
using namespace rpg2kLib::encode;
using rpg2kLib::structure::Binary ;
using rpg2kLib::structure::Element;
 */


namespace rpg2kLib
{
	namespace debug
	{

std::string getError(int errNum)
{
	const char* message = strerror(errNum);
	switch(errno) {
		case EINVAL: throw std::invalid_argument("Invalid Error Descriptor.");
		default: return message;
	}
}
void addAtExitFunction( void (*func)(void) )
{
	if( atexit(func) != 0 ) throw std::runtime_error("atexit() error.");
}
/*
std::exception getException(const void* pthis)
{
#if USE_RTTI
	return std::runtime_error(
		 "Exception at " + demangleTypeInfo( typeid( *( (int*)pthis ) ) )
	);
#else
	return std::runtime_error("Exception");
#endif
}
 */

std::string demangleTypeInfo(const std::type_info& info)
{
	int status;
	char* readable =  abi::__cxa_demangle( info.name(), NULL, NULL, &status );
// errors
	if(readable) throw std::runtime_error("Demangling failed.");
	switch(status) {
		case -1: throw std::runtime_error("Memory error.");
		case -2: throw std::runtime_error("Invalid name.");
		case -3: throw std::runtime_error("Argument was invalid");
		default: break;
	}
// char* to string
	std::string ret(readable);
	free(readable);

	return ret;
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

/*
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
			ostrm << std::dec << cur.getIndex1() << "]";
			try {
				uint index2 = cur.getIndex2();
				ostrm << "[" << std::dec << index2 << "]";
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
		if( b.size() == 0 ) {
			ostrm << "\t" "This data is empty.\n";
			return;
		} else {
		// Binary
			ostrm << "\t" "Binary: size = ";
			ostrm << std::setw(8) << b.size() << "; data = { ";

			ostrm.fill('0');
			for(uint i = 0; i < b.size(); i++) {
				ostrm << std::setw(2) << std::hex << ( b[i] & 0xff ) << " ";
			}
			ostrm.fill(' ');

			ostrm << "};\n";
		// BER number
			if( b.isNumber() ) {
				ostrm << "\t" "BER: ";
				ostrm << std::setw(8) << std::dec << (int) b.toNumber() << ";\n";
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
			ostrm << std::setw(8) << b.size() << "; data = { ";

			ostrm.fill('0');
			for(uint i = 0; i < b.size(); i++) {
				ostrm << std::setw(2) << std::hex << (b[i] & 0xff) << " ";
			}
			ostrm.fill(' ');

			ostrm << "}";
		} else if(type == "bool") { ostrm << std::boolalpha << e.get_bool();
		} else if(type == "double") { ostrm << e.get_double();
		} else if(type == "string") {
			try {
				ostrm << "\"" << Encode::getInstance().toSystem(e) << "\"";
			} catch(...) {
				ostrm << "\"\"";
			}
		} else if(type == "int") { ostrm << std::dec << e.get_int();
		} else ostrm << type;

		ostrm << ";" << std::endl;
	}
}
 */

	}; // namespace debug
}; // namespace rpg2kLib
