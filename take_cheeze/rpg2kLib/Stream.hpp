#ifndef _INC__RPG2k__FILE_HPP
#define _INC__RPG2k__FILE_HPP

#include "Struct.hpp"

#include <string>
#include <typeinfo>

#include <stdio.h>

namespace rpg2kLib
{
	namespace structure
	{

		using namespace std;

		class StreamInterface
		{
		protected:
			StreamInterface() {}
			StreamInterface(const StreamInterface& i) {}
		public:
			virtual ~StreamInterface() {}

			virtual string name() const = 0;

			virtual uint seekFromSet(uint val) = 0;
			virtual uint seekFromCur(uint val) = 0;
			virtual uint seekFromEnd(uint val) = 0;

			virtual uint length() = 0;

			virtual uint tell() = 0;

			virtual uint8_t read() { throw logic_error("Unimplemented"); }
			virtual uint read(uint8_t* data, uint size) { throw logic_error("Unimplemented"); }

			virtual bool eof() { throw logic_error("Unimplemented"); }

			virtual void write(uint8_t data) { throw logic_error("Unimplemented"); }
			virtual uint write(uint8_t* data, uint size) { throw logic_error("Unimplemented"); }

			virtual void resize(uint size) { throw logic_error("Unimplemented"); }
		};

		class FileInterface : public StreamInterface
		{
		private:
			FILE* FILE_POINTER;
			string NAME;
		protected:
			FILE* getFilePointer() { return FILE_POINTER; }
			FileInterface(string filename, const char* mode);
		public:
			virtual ~FileInterface();

			string name() const { return NAME; }

			uint seekFromSet(uint val);
			uint seekFromCur(uint val);
			uint seekFromEnd(uint val);

			uint length();

			uint tell() { return ftell(FILE_POINTER); }
		};
		class FileReader : public FileInterface
		{
		public:
			using FileInterface::length;
			using FileInterface::name;

			FileReader(string name);
			virtual ~FileReader();

			uint8_t read();
			uint read(uint8_t* data, uint size);

			bool eof() { return feof( getFilePointer() ); }
		};
		class FileWriter : public FileInterface
		{
		public:
			FileWriter(string name);
			virtual ~FileWriter();

			using FileInterface::length;

			void resize(uint size) {}

			void write(uint8_t data);
			uint write(uint8_t* data, uint size);
		};

		class BinaryInterface : public StreamInterface
		{
		private:
			uint SEEK;
			Binary& BINARY;
		protected:
			Binary& getBinary() { return BINARY; }
			uint& getSeek() { return SEEK; }
		public:
			BinaryInterface(Binary& b);
			virtual ~BinaryInterface() {}

			string name() const { return "Binary"; }

			uint seekFromSet(uint val);
			uint seekFromCur(uint val);
			uint seekFromEnd(uint val);

			uint length() { return BINARY.length(); }

			uint tell() { return SEEK; }
		};

		class BinaryWriter : public BinaryInterface
		{
		public:
			using BinaryInterface::length;
			using BinaryInterface::tell;

			void resize(uint size) { getBinary().resize(size); }

			void write(uint8_t data);
			uint write(uint8_t* data, uint size);
		};
		class BinaryReader : public BinaryInterface
		{
		public:
			using BinaryInterface::length;
			using BinaryInterface::tell;

			uint8_t read();
			uint read(uint8_t* data, uint size);

			bool eof() { return tell() >= length(); }
		};

		class StreamReader
		{
		private:
			StreamInterface& IMPLEMENT;
			bool AUTO_RELEASE;

			StreamReader();

			void open(Binary& bin);
		protected:
			StreamInterface& getImplement() { return IMPLEMENT; }
		public:
			StreamReader(StreamInterface& imp, bool autoRelease = true);
			virtual ~StreamReader();

			StreamReader(Binary& bin);
			StreamReader(string  str);

			void close();

			uint length() { return IMPLEMENT.length(); }

			string name() const { return IMPLEMENT.name(); }

			uint seekFromSet(uint val = 0) { return IMPLEMENT.seekFromSet(val); }
			uint seekFromCur(uint val = 0) { return IMPLEMENT.seekFromCur(val); }
			uint seekFromEnd(uint val = 0) { return IMPLEMENT.seekFromEnd(val); }
			uint seek(uint pos = 0) { return seekFromSet(pos); }

			uint tell() { return IMPLEMENT.tell(); }

			bool eof() { return IMPLEMENT.eof(); }

			uint8_t read() { return IMPLEMENT.read(); }
			uint read(uint8_t* data, uint size) { return IMPLEMENT.read(data, size); }
		// read and copy to argument
		// size would be the length of argument
			uint read(Binary& b) { return IMPLEMENT.read( b.getPtr(), b.length() ); }

			uint32_t getBER();

			Binary& get(Binary& b) { b.reset( getBER() ); read(b); return b; }

			StreamReader& operator >>( int& num) { num = getBER(); return *this; }
			StreamReader& operator >>(uint& num) { num = getBER(); return *this; }
			StreamReader& operator >>(Binary& b) { read(b); return *this; }
			StreamReader& operator >=(Binary& b) { get(b); return *this; }

			bool checkHeader(string header);
		};

		class StreamWriter
		{
		private:
			StreamInterface& IMPLEMENT;
			bool AUTO_RELEASE;
		protected:
			StreamInterface& getImplement() { return IMPLEMENT; }
		public:
			StreamWriter(StreamInterface& imp, bool autoRelease = true);
			virtual ~StreamWriter();

			StreamWriter(Binary& bin);
			StreamWriter(string  str);

			void close();

			uint length() { return IMPLEMENT.length(); }
			void resize(uint size) { IMPLEMENT.resize(size); }

			string name() const { return IMPLEMENT.name(); }

			uint seekFromSet(uint val = 0) { return IMPLEMENT.seekFromSet(val); }
			uint seekFromCur(uint val = 0) { return IMPLEMENT.seekFromCur(val); }
			uint seekFromEnd(uint val = 0) { return IMPLEMENT.seekFromEnd(val); }
			uint seek(uint pos = 0) { return seekFromSet(pos); }

			uint tell() { return IMPLEMENT.tell(); }

			StreamWriter& operator <<( int num) { setBER(num); return *this; }
			StreamWriter& operator <<(uint num) { setBER(num); return *this; }
			StreamWriter& operator <<(const Binary& b) { write(b); return *this; }
			StreamWriter& operator <=(const Binary& b) { set(b); return *this; }

			void write(uint8_t data);
			uint write(uint8_t* data, uint size);
			uint write(const Binary& b);

			uint setBER(uint32_t num);
			void set(const Binary& b) { setBER( b.length() ); write(b); }

			void setHeader(string header) { seekFromSet(); set( Binary(header) ); }
		};

	}; // namespace structure
}; // namespace rpg2kLib

#endif
