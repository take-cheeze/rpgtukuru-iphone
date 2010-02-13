#ifndef _INC__RPG2k__FILE_HPP
#define _INC__RPG2k__FILE_HPP

#include "Struct.hpp"

#include <string>
#include <typeinfo>

#include <fcntl.h>

#if defined PSP
	#include <pspiofilemgr.h>
	#define __open  sceIoOpen
	#define __close sceIoClose
	#define __lseek sceIoLseek
	#define __read  sceIoRead
	#define __write sceIoWrite
#else
	#define __open  ::open
	#define __close ::close
	#define __lseek ::lseek
	#define __read  ::read
	#define __write ::write
#endif

namespace rpg2kLib
{
	namespace structure
	{

		using namespace std;

		class StreamInterface
		{
		protected:
			StreamInterface() {}
			StreamInterface(const StreamInterface& i);
		public:
			virtual ~StreamInterface();

			virtual string name() const = 0;

			virtual uint seekFromSet(uint val = 0) = 0;
			virtual uint seekFromCur(uint val = 0) = 0;
			virtual uint seekFromEnd(uint val = 0) = 0;

			virtual uint8_t read() = 0;
			virtual uint read(uint8_t* data, uint size) = 0;

			virtual uint length() = 0;
			virtual void resize(uint size) = 0;

			virtual void write(uint8_t data) = 0;
			virtual uint write(uint8_t* data, uint size) = 0;
		};

		class FileImplement : public StreamInterface
		{
		protected:
			int fileStatus() { return O_CREAT; }
			mode_t fileMode() { return S_IREAD | S_IWRITE; }

			int HANDLE;
			string NAME;
		public:
			FileImplement(string filename);
			virtual ~FileImplement() throw(std::string);

			int getHandle() const { return HANDLE; }
			string name() const { return NAME; }

			uint seekFromSet(uint val = 0) { return __lseek(HANDLE, val, SEEK_SET); }
			uint seekFromCur(uint val = 0) { return __lseek(HANDLE, val, SEEK_CUR); }
			uint seekFromEnd(uint val = 0) { return __lseek(HANDLE, val, SEEK_END); }

			uint8_t read();
			uint read(uint8_t* data, uint size) { return __read(HANDLE, data, size); }

			uint length();
#if defined PSP
			void resize(uint size) {}
#else
			void resize(uint size);
#endif

			void write(uint8_t data);
			uint write(uint8_t* data, uint size) { return __write(HANDLE, data, size); }
		};

		class BinaryImplement : public StreamInterface
		{
		protected:
			uint SEEK;
			Binary* BINARY;
		public:
			BinaryImplement(Binary& b);
			virtual ~BinaryImplement() {}

			string name() const { return "Binary"; }

			uint seekFromSet(uint val = 0);
			uint seekFromCur(uint val = 0);
			uint seekFromEnd(uint val = 0);

			uint8_t read();
			uint read(uint8_t* data, uint size);

			uint length() { return BINARY->length(); }
			void resize(uint size) { BINARY->resize(size); }

			void write(uint8_t data);
			uint write(uint8_t* data, uint size);
		};

		class Stream
		{
		private:
			StreamInterface* IMPLEMENT;

			void open(Binary& bin);
		public:
			Stream();
			virtual ~Stream();

			void open(string name);

			Stream(Binary& bin) : IMPLEMENT(NULL) { open(bin); }
			Stream(string  str) : IMPLEMENT(NULL) { open(str); }

			void close();

			uint length() { return IMPLEMENT->length(); }
			void resize(uint size) { IMPLEMENT->resize(size); }

			string name() const { return IMPLEMENT->name(); }

			uint seekFromSet(uint val = 0) { return IMPLEMENT->seekFromSet(val); }
			uint seekFromCur(uint val = 0) { return IMPLEMENT->seekFromCur(val); }
			uint seekFromEnd(uint val = 0) { return IMPLEMENT->seekFromEnd(val); }
			uint seek(uint pos = 0) { return seekFromSet(pos); }

			uint tell() { return seekFromCur(); }

			bool eof() { return (seekFromCur() + 1) >= length(); }

			uint8_t read() { return IMPLEMENT->read(); }
			uint read(uint8_t* data, uint size) { return IMPLEMENT->read(data, size); }
		// read and copy to argument
		// size would be the length of argument
			uint read(Binary& b) { return IMPLEMENT->read( b.getPtr(), b.length() ); }

			void write(uint8_t data);
			uint write(uint8_t* data, uint size);
			uint write(const Binary& b);

			uint32_t getBER();
			uint setBER(uint32_t num);

			Binary& get(Binary& b) { b.reset( getBER() ); read(b); return b; }
			void set(const Binary& b) { setBER( b.length() ); write(b); }

			Stream& operator <<( int num) { setBER(num); return *this; }
			Stream& operator <<(uint num) { setBER(num); return *this; }
			Stream& operator <<(const Binary& b) { write(b); return *this; }
			Stream& operator <=(const Binary& b) { set(b); return *this; }

			Stream& operator >>( int& num) { num = getBER(); return *this; }
			Stream& operator >>(uint& num) { num = getBER(); return *this; }
			Stream& operator >>(Binary& b) { read(b); return *this; }
			Stream& operator >=(Binary& b) { get(b); return *this; }

			string typeName() const { return typeid(*IMPLEMENT).name(); }

			void setHeader(string header) { seekFromSet(); set( Binary(header) ); }
			bool checkHeader(string header);
		};

		inline bool isBinary(Stream& s) { return s.typeName() == typeid(BinaryImplement).name(); }
		inline bool isFile  (Stream& s) { return s.typeName() == typeid(  FileImplement).name(); }

	}; // namespace structure
}; // namespace rpg2kLib

#endif
