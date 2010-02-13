#include "Debug.hpp"
#include "Stream.hpp"

using namespace rpg2kLib::debug;
using namespace rpg2kLib::structure;

Stream::Stream() : IMPLEMENT(NULL)
{
}
Stream::~Stream()
{
	close();
}

void Stream::open(string name)
{
	close();
	IMPLEMENT = new FileImplement(name);

	cout << "Stream::open(" << name << ");" << endl;
}
void Stream::open(Binary& bin)
{
	close();
	IMPLEMENT = new BinaryImplement(bin);
/*
	clog << typeid(*IMPLEMENT).name() << endl;
	clog << typeid(BinaryImplement).name() << endl;
 */
}

void Stream::close()
{
	if(IMPLEMENT != NULL) {
		delete IMPLEMENT;
		IMPLEMENT = NULL;
	}
}

void Stream::write(uint8_t data)
{
	if( tell() >= length() ) resize( tell() + sizeof(uint8_t) );

	IMPLEMENT->write(data);
}
uint Stream::write(uint8_t* data, uint size)
{
	if( ( tell()+size ) > length() ) resize( tell()+size );

	return IMPLEMENT->write(data, size);
}
uint Stream::write(const Binary& b)
{
	if( ( tell()+b.length() ) > length() ) resize( tell()+b.length() );
	return IMPLEMENT->write( b.getPtr(), b.length() );
}

uint32_t Stream::getBER()
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
uint Stream::setBER(uint32_t num)
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
bool Stream::checkHeader(string header)
{
	Binary buf;

/*
	get(buf);
	clog << buf.length() << endl;
	string str = buf;
	clog << str << endl;
	return str == header;
 */

	return ( (string) get(buf) ) == header;
}

StreamInterface::~StreamInterface()
{
}

FileImplement::FileImplement(string filename)
	: StreamInterface(), NAME(filename)
{
	HANDLE = __open( NAME.c_str(), fileStatus(), fileMode() );
	if(HANDLE == -1) {
		int errnoBuf = errno;
		throw "Error at Stream.open(" + name() + "): " + getError(errnoBuf);
	}
	seekFromSet();
}
FileImplement::~FileImplement() throw(std::string)
{
	if( __close(HANDLE) == -1 ) {
		int errnoBuf = errno;
		throw "Error at Stream(" + name() + ").close() :" + getError(errnoBuf);
	}
}
uint FileImplement::length()
{
	uint cur = seekFromCur(0), ret = seekFromEnd(0);
	seekFromSet(cur);

	return ret + 1;
}

#ifndef PSP
void FileImplement::resize(uint size)
{
	if( ftruncate(HANDLE, size) == -1 ) {
		int errnoBuf = errno;
		throw "Error at Stream(" + NAME + ").resize() :" + getError(errnoBuf);
	}
}
#endif

uint8_t FileImplement::read()
{
	uint8_t ret = 0;
	if( __read( HANDLE, &ret, sizeof(ret) ) != sizeof(ret) )
		throw "Stream(" + name() + ") has reached EOF.";
	else return ret;
}
void FileImplement::write(uint8_t data)
{
	if( __write( HANDLE, &data, sizeof(data) ) != sizeof(data) ) {
		int errnoBuf = errno;
		throw getError(errnoBuf);
	}
}

BinaryImplement::BinaryImplement(Binary& b)
	: StreamInterface(), SEEK(0), BINARY(&b)
{
	seekFromSet();
}
uint BinaryImplement::seekFromSet(uint val)
{
	if( val > BINARY->length() ) SEEK = BINARY->length();
	else SEEK = val;

	return SEEK;
}
uint BinaryImplement::seekFromCur(uint val)
{
	if( (SEEK + val) > BINARY->length() ) SEEK = BINARY->length();
	else SEEK += val;

	return SEEK;
}
uint BinaryImplement::seekFromEnd(uint val)
{
	if( (BINARY->length() - val) > BINARY->length() ) SEEK = 0;
	else SEEK = BINARY->length() - val - 1;

	return SEEK;
}
uint8_t BinaryImplement::read()
{
	if( SEEK < BINARY->length() ) return (*BINARY)[SEEK++];
	else throw "Stream(" + name() + ") has reached EOF.";
}
uint BinaryImplement::read(uint8_t* data, uint size)
{
	uint ret;
	if( (SEEK + size) >= BINARY->length() ) ret = BINARY->length()-SEEK;
	else ret = size;

	memcpy(data, BINARY->getPtr(SEEK), ret);

	SEEK += ret;
	return ret;
}
void BinaryImplement::write(uint8_t data)
{
	(*BINARY)[SEEK++] = data;
}
uint BinaryImplement::write(uint8_t* data, uint size)
{
	if( (SEEK+size) > length() ) throw "Writing data is too big.";

	memcpy( BINARY->getPtr(SEEK), data, size );
	SEEK += size;

	return size;
}
