#include "Debug.hpp"
#include "Stream.hpp"

using namespace rpg2kLib::debug;
using namespace rpg2kLib::structure;

StreamWriter::StreamWriter(StreamInterface& imp, bool autoRelease)
	: IMPLEMENT(imp), AUTO_RELEASE(autoRelease)
{
}
StreamReader::StreamReader(StreamInterface& imp, bool autoRelease)
	: IMPLEMENT(imp), AUTO_RELEASE(autoRelease)
{
}
StreamWriter::StreamWriter(string name)
	: IMPLEMENT( *new FileWriter(name) ), AUTO_RELEASE(true)
{
}
StreamReader::StreamReader(string name)
	: IMPLEMENT( *new FileReader(name) ), AUTO_RELEASE(true)
{
}
StreamWriter::StreamWriter(Binary& bin)
	: IMPLEMENT( *new BinaryWriter(bin) ), AUTO_RELEASE(true)
{
}
StreamReader::StreamReader(Binary& bin)
	: IMPLEMENT( *new BinaryReader(bin) ), AUTO_RELEASE(true)
{
}
StreamWriter::~StreamWriter()
{
	if(AUTO_RELEASE) delete &IMPLEMENT;
}
StreamReader::~StreamReader()
{
	if(AUTO_RELEASE) delete &IMPLEMENT;
}

uint8_t StreamReader::read()
{
	if( length() <= tell() )
		throw length_error("Reached end of stream");
	else return IMPLEMENT.read();
}
uint StreamReader::read(uint8_t* data, uint size)
{
	if( length() < ( tell()+size ) )
		throw length_error("Reached end of stream");
	else return IMPLEMENT.read(data, size);
}
uint StreamReader::read(Binary& b)
{
	if( length() < ( tell()+b.length() ) )
		throw length_error("Reached end of stream");
	else return IMPLEMENT.read( b.getPtr(), b.length() );
}

void StreamWriter::write(uint8_t data)
{
	if( length() <= tell() ) resize( tell() + sizeof(uint8_t) );
	IMPLEMENT.write(data);
}
uint StreamWriter::write(uint8_t* data, uint size)
{
	if( length() < ( tell()+size ) ) resize( tell()+size );
	return IMPLEMENT.write(data, size);
}
uint StreamWriter::write(const Binary& b)
{
	if( length() < ( tell()+b.length() ) ) resize( tell()+b.length() );
	return IMPLEMENT.write( b.getPtr(), b.length() );
}

uint32_t StreamReader::getBER()
{
	uint32_t ret = 0;
	uint8_t data;
// extract
	do {
		data = this->read();
		ret = (ret << BER_BIT) | (data & BER_MASK);
	} while(data > BER_SIGN);
// result
	return ret;
}
uint StreamWriter::setBER(uint32_t num)
{
	uint8_t buff[ ( sizeof(num) * CHAR_BIT ) / BER_BIT + 1];
	uint size = getBERSize(num), index = size;
// set data
	buff[--index] = num & BER_MASK;
	num >>= BER_BIT;
	while(!num) {
		buff[--index] = (num & BER_MASK) | BER_SIGN;
		num >>= BER_BIT;
	}
// write data
	write(buff, size);
	return size;
}
bool StreamReader::checkHeader(string header)
{
	Binary buf;
	return static_cast< string >( get(buf) ) == header;
}

BinaryReader::BinaryReader(Binary& bin) : BinaryInterface(bin)
{
}
BinaryWriter::BinaryWriter(Binary& bin) : BinaryInterface(bin)
{
}

FileInterface::FileInterface(string filename, const char* mode)
	: NAME(filename)
{
	FILE_POINTER = fopen( filename.c_str(), mode );
	if(FILE_POINTER == NULL) {
		int errnoBuf = errno;
		throw "Error at Stream.open(" + name() + "): " + getError(errnoBuf);
	}
}

uint FileInterface::seekFromSet(uint val)
{
	return fseek(FILE_POINTER, val, SEEK_SET);
}
uint FileInterface::seekFromCur(uint val)
{
	return fseek(FILE_POINTER, val, SEEK_CUR);
}
uint FileInterface::seekFromEnd(uint val)
{
	return fseek(FILE_POINTER, val, SEEK_END);
}

FileReader::FileReader(string name) : FileInterface(name, "rb")
{
}
FileReader::~FileReader()
{
}
FileWriter::FileWriter(string name) : FileInterface(name, "w+b")
{
}
FileWriter::~FileWriter()
{
}
uint FileReader::read(uint8_t* data, uint size)
{
	return fread( data, sizeof(uint8_t), size, getFilePointer() );
}
uint8_t FileReader::read()
{
	uint8_t ret = 0;
	if( fread( &ret, sizeof(ret), 1, getFilePointer() ) != sizeof(ret) ) {
		int errnoBuf = errno;
		throw getError(errnoBuf);
	}
	else return ret;
}
uint FileWriter::write(uint8_t* data, uint size)
{
	return fwrite( data, sizeof(uint8_t), size, getFilePointer() );
}
void FileWriter::write(uint8_t data)
{
	if( fwrite( &data, sizeof(data), 1, getFilePointer() ) != sizeof(data) ) {
		int errnoBuf = errno;
		throw getError(errnoBuf);
	}
}

FileInterface::~FileInterface()
{
	if( fclose(FILE_POINTER) == EOF ) {
		int errnoBuf = errno;
		throw "Error at Stream(" + name() + ").close() :" + getError(errnoBuf);
	}
}
uint FileInterface::length()
{
	uint cur = seekFromCur(0), ret = seekFromEnd(0);
	seekFromSet(cur);

	return ret + 1;
}

BinaryInterface::BinaryInterface(Binary& b)
	: SEEK(NULL), BINARY(b)
{
}
uint BinaryInterface::seekFromSet(uint val)
{
	if( val > length() ) getSeek() = length();
	else getSeek() = val;

	return getSeek();
}
uint BinaryInterface::seekFromCur(uint val)
{
	if( (getSeek() + val) > length() ) getSeek() = length();
	else getSeek() += val;

	return getSeek();
}
uint BinaryInterface::seekFromEnd(uint val)
{
	uint len = length();

	if( (len - val) > len ) getSeek() = 0;
	else getSeek() = len - val - 1;

	return getSeek();
}
uint8_t BinaryReader::read()
{
	return getBinary()[getSeek()++];
}
uint BinaryReader::read(uint8_t* data, uint size)
{
	uint& seek = getSeek();

	memcpy( data, getBinary().getPtr(seek), size );
	seek += size;

	return size;
}
void BinaryWriter::write(uint8_t data)
{
	getBinary().getPtr()[getSeek()++] = data;
}
uint BinaryWriter::write(uint8_t* data, uint size)
{
	uint& seek = getSeek();

	memcpy( getBinary().getPtr(seek), data, size );
	seek += size;

	return size;
}
