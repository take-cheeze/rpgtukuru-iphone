#include <cstdlib>

#include <algorithm>
#include <functional>
#include <iterator>
#include <stack>

#include <boost/scoped_ptr.hpp>

#include "Array1D.hpp"
#include "Array2D.hpp"
#include "Debug.hpp"
#include "Element.hpp"
#include "Event.hpp"
#include "Stream.hpp"

// demangling header
#if (RPG2K_IS_GCC || RPG2K_IS_CLANG)
	#include <cxxabi.h>
#elif RPG2K_IS_MSVC
	extern "C"
	char * _unDName(
		char * outputString,
		const char * name,
		int maxStringLength,
		void * (* pAlloc )(size_t),
		void (* pFree )(void *),
		unsigned short disableFlags
	);
#endif

using rpg2k::structure::Array1D;
using rpg2k::structure::Array2D;
using rpg2k::structure::Element;
using rpg2k::structure::ElementType;


namespace rpg2k
{
	namespace debug
	{
		std::string error(int const errNo)
		{
			char const* message = strerror(errNo);
			switch(errno) {
				case EINVAL: rpg2k_assert(false);
				default: return message;
			}
		}
		void addAtExitFunction( void (*func)(void) )
		{
			if( atexit(func) != 0 ) rpg2k_assert(false);
		}

		std::string demangleTypeInfo(std::type_info const& info)
		{
			#if (RPG2K_IS_GCC || RPG2K_IS_CLANG)
				int status;
				char* const readable = abi::__cxa_demangle( info.name(), NULL, NULL, &status );

				rpg2k_assert(readable);
				switch(status) {
					case -1: rpg2k_assert(!"Memory error.");
					case -2: rpg2k_assert(!"Invalid name.");
					case -3: rpg2k_assert(!"Argument was invalid");
					default: break;
				}
				std::string ret = readable; // char* to string
				std::free(readable);
			#elif RPG2K_IS_MSVC
				char* const readable = _unDName( 0, info.name(), 0, std::malloc, std::free, 0x2800 );
				rpg2k_assert(readable);
				std::string ret = readable; // char* to string
				std::free(readabl);
			#endif

			return ret;
		}

		std::ofstream ANALYZE_RESULT("analyze.txt");

		std::ostream& Tracer::printTrace(structure::Element const& e, bool const info, std::ostream& ostrm)
		{
			std::stack<Element const*> st;

			for( Element const* buf = &e; buf->hasOwner(); buf = &( buf->owner() ) ) {
				st.push(buf);
			}

			for(; !st.empty(); st.pop()) {
				Element const& top = *st.top();

				ElementType::Enum const ownerType = top.owner().descriptor().type();

				ostrm << std::dec << std::setfill(' ');
				ostrm << ElementType::instance().toString(ownerType)
					<< "[" << std::setw(4) << top.index1() << "]";
				if( ownerType == ElementType::Array2D_ ) ostrm
					<< "[" << std::setw(4) << top.index2() << "]";
				ostrm << ": ";
			}

			if(info) { Tracer::printInfo(e, ostrm); }

			return ostrm;
		}

		std::ostream& Tracer::printInfo(structure::Element const& e, std::ostream& ostrm)
		{
			using structure::ArrayDefinePointer;
			using structure::ArrayDefineType;
			using structure::Descriptor;

			if( e.isDefined() ) {
				ostrm << e.descriptor().typeName() << ": ";

				switch( e.descriptor().type() ) {
					case ElementType::Binary_: printBinary(e, ostrm); break;
					case ElementType::Event_ : printEvent (e, ostrm); break;
					case ElementType::bool_  : printBool  (e.to<bool>(), ostrm); break;
					case ElementType::double_: printDouble(e, ostrm); break;
					case ElementType::string_: printString(e, ostrm); break;
					case ElementType::int_   : printInt   (e, ostrm); break;
					default: break;
				}
			} else {
				Binary const bin = e.serialize();
				if( bin.size() == 0 ) {
					ostrm << "This data is empty." << endl;
					return ostrm;
				}
			// Binary
				ostrm << endl << "Binary: ";
				printBinary(bin, ostrm);
			// Event
				try {
					structure::Event event(bin);
					ostrm << endl << "Event: ";
					printEvent(event, ostrm);
				} catch(...) {}
			// BER number
				if( bin.isNumber() ) {
					ostrm << endl << "BER: ";
					printInt(bin, ostrm);
				}
			// string
				if( bin.isString() ) {
					ostrm << endl << "string: ";
					printString(bin, ostrm);
				}
			// Array1D
				try {
					boost::scoped_ptr<Element> p( new Element( Descriptor(
						ElementType::instance().toString(ElementType::Array1D_),
						ArrayDefinePointer(new ArrayDefineType) ), bin) );
					ostrm << endl << "---Array1D check start---" << endl;
					p.reset();
					ostrm << "---Array1D check end  ---";
				} catch(...) {}
			// Array2D
				try {
					boost::scoped_ptr<Element> p( new Element( Descriptor(
						ElementType::instance().toString(ElementType::Array2D_),
						ArrayDefinePointer(new ArrayDefineType) ), bin) );
					ostrm << endl << "---Array2D check start---" << endl;
					p.reset();
					ostrm << "---Array2D check end  ---";
				} catch(...) {}
			}

			ostrm << endl;

			return ostrm;
		}

		std::ostream& Tracer::printArray1D(structure::Array1D const& val, std::ostream& ostrm)
		{
			std::map<unsigned, Element const*> buf;
			for(Array1D::ConstIterator i = val.end(); i != val.end(); ++i) {
				buf.insert( std::make_pair(i->first, i->second) );
			}
			for(std::map<unsigned, Element const*>::const_iterator i = buf.begin(); i != buf.end(); ++i) {
				printTrace( *(i->second), true, ostrm );
			}
			return ostrm;
		}
		std::ostream& Tracer::printArray2D(structure::Array2D const& val, std::ostream& ostrm)
		{
			for(Array2D::ConstIterator i = val.begin(); i != val.end(); ++i) {
				printArray1D(*i->second, ostrm);
			}
			return ostrm;
		}
		std::ostream& Tracer::printInt(int const val, std::ostream& ostrm)
		{
			ostrm << std::dec << val;
			return ostrm;
		}
		std::ostream& Tracer::printBool(bool const val, std::ostream& ostrm)
		{
			ostrm << std::boolalpha << val;
			return ostrm;
		}
		std::ostream& Tracer::printDouble(double const val, std::ostream& ostrm)
		{
			ostrm << std::showpoint << val;
			return ostrm;
		}
		std::ostream& Tracer::printString(RPG2kString const& val, std::ostream& ostrm)
		{
			ostrm << "\"" << val.toSystem() << "\"";
			return ostrm;
		}
		std::ostream& Tracer::printEvent(structure::Event const& val, std::ostream& ostrm)
		{
			ostrm << std::dec << std::setfill(' ');
			ostrm << "size = " << val.serializedSize() << "; data = {";

			for(unsigned i = 0; i < val.size(); i++) {
				ostrm << endl << "\t";
				printInstruction(val[i], ostrm, true);
			}

			ostrm << endl << "}";

			return ostrm;
		}
		std::ostream& Tracer::printInstruction(structure::Instruction const& inst
		, std::ostream& ostrm, bool indent)
		{
			if(indent) for(unsigned i = 0; i < inst.nest(); i++) ostrm << "\t";
			ostrm << "{ "
				<< "nest: " << std::setw(4) << std::dec << inst.nest() << ", "
				<< "code: " << std::setw(5) << std::dec << inst.code() << ", "
				<< "string: \"" << inst.string().toSystem() << "\", "
				<< "integer[" << inst.argNum() << "]: "
				;
				ostrm << "{ ";
					std::copy( inst.args().begin(), inst.args().end()
					, std::ostream_iterator<int32_t>(ostrm, ", ") );
				ostrm << "}, ";
			ostrm << "}";

			return ostrm;
		}
		std::ostream& Tracer::printBinary(Binary const& val, std::ostream& ostrm)
		{
			ostrm << std::setfill(' ') << std::dec;
			ostrm << "size = " << val.size() << "; data = { ";

			ostrm << std::setfill('0') << std::hex;
			for(unsigned i = 0; i < val.size(); i++) ostrm << std::setw(2) << (val[i] & 0xff) << " ";

			ostrm << "}";

			return ostrm;
		}
	} // namespace debug
} // namespace rpg2k
