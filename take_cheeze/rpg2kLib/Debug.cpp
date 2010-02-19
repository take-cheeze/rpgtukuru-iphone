#include <cstdlib>

#include <stack>
#include <list>

#include "Debug.hpp"
#include "Encode.hpp"
#include "Element.hpp"

using namespace rpg2kLib::debug;
using namespace rpg2kLib::encode;

string rpg2kLib::debug::getError(int errNum)
{
	const char* message = strerror(errNum);
	switch(errno) {
		case EINVAL: throw "Invalid Error Descriptor.";
		default: return message;
	}
}
void rpg2kLib::debug::addAtExitFunction( void (*func)(void) )
{
	if( atexit(func) != 0 ) throw "atexit() error.";
}

char Debug::COUT_BUF[BUF_SIZE], Debug::CERR_BUF[BUF_SIZE];
stack< streambuf* > Debug::COUT_FB_STACK, Debug::CERR_FB_STACK;

void Debug::init() {
	filebuf* fb = new filebuf();
	fb->open(COUT.c_str(), ios::out);
	fb->pubsetbuf(COUT_BUF, BUF_SIZE);
	COUT_FB_STACK.push( cout.rdbuf(fb) );

	fb = new filebuf();
	fb->open(CERR.c_str(), ios::out);
	fb->pubsetbuf(CERR_BUF, BUF_SIZE);
	CERR_FB_STACK.push( cerr.rdbuf(fb) );

	cout << "Debug start at ";
	cerr << "Debug start at ";
#if defined(PSP)
	cout << "PSP." << endl << endl;
	cerr << "PSP." << endl << endl;
#else
	cout << "PC." << endl << endl;
	cerr << "PC." << endl << endl;
#endif
}
void Debug::dispose() {
	if( !COUT_FB_STACK.empty() && !CERR_FB_STACK.empty() ) {
		delete (filebuf*) cout.rdbuf( COUT_FB_STACK.top() );
		delete (filebuf*) cerr.rdbuf( CERR_FB_STACK.top() );

		COUT_FB_STACK.pop();
		CERR_FB_STACK.pop();
	}
}

void Tracer::printTrace(Element& e, bool info, ostream& ostrm)
{
	stack< Element*, list< Element* > > st;

	st.push(&e);
	for( Element* buf = st.top(); buf->hasOwner(); buf = st.top() ) {
		// clog << hex << st.top() << endl;
		st.push( &buf->getOwner() );
	}

	while( !st.empty() ) {
		Element& cur = *( st.top() ); st.pop();

		if( cur.hasOwner() ) {
			ostrm << cur.getOwner().getDescriptor().getTypeName() << "[";
			/* ostrm.width(6); */ ostrm << dec << cur.getIndex1() << "]";
			try {
				uint index2 = cur.getIndex2();
				ostrm << "["; /* ostrm.width(6); */ ostrm << dec << index2 << "]";
			} catch(...) {}
			ostrm << ": ";
		}
	}

	if(info) Tracer::printInfo(e, ostrm);
}

void Tracer::printInfo(Element& e, ostream& ostrm)
{
	if( e.isUndefined() ) {
		ostrm << endl;
		const Binary& b = e.toBinary();
	// check whether it's empty
		if( b.length() == 0 ) {
			ostrm << "\t" "This data is empty." << endl;
			return;
		} else {
		// Binary
			ostrm << "\t" "Binary: size = ";
			ostrm.width(8); ostrm << b.length() << "; data = { ";

			ostrm.fill('0');
			for(uint i = 0; i < b.length(); i++) {
				ostrm.width(2); ostrm << hex << (b[i] & 0xff) << " ";
			}
			ostrm.fill(' ');

			ostrm << "};" << endl;
		// BER number
			if( b.isNumber() ) {
				ostrm << "\t" "BER: ";
				ostrm.width(8); ostrm << dec << (int) b.toNumber() << ";" << endl;
			}
		// string
			if( Encode::getInstance().isString(b) ) {
				try {
					string str = Encode::getInstance().toSystem(b);
					ostrm << "\t" "string: " << str << ";" << endl;
				} catch(...) {}
			}
		}
	} else {
		string type = e.getDescriptor().getTypeName();
		if(type == "Binary") {
			ostrm << endl;
			Binary& b = e;

			ostrm << "\t" << type << ": size = ";
			ostrm.width(8); ostrm << b.length() << "; data = { ";

			ostrm.fill('0');
			for(uint i = 0; i < b.length(); i++) {
				ostrm.width(2); ostrm << hex << (b[i] & 0xff) << " ";
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
			ostrm << dec << (int)e;
		} else ostrm << type;

		ostrm << ";" << endl;
	}
}
