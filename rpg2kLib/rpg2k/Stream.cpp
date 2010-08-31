#include "Debug.hpp"
#include "Stream.hpp"


namespace rpg2k
{
	namespace structure
	{
		StreamWriter::StreamWriter(std::auto_ptr< StreamInterface > imp)
		: implement_(imp)
		{
		}
		StreamReader::StreamReader(std::auto_ptr< StreamInterface > imp)
		: implement_(imp)
		{
		}
		StreamWriter::StreamWriter(SystemString const& name)
		: implement_( new FileWriter(name) )
		{
		}
		StreamReader::StreamReader(SystemString const& name)
		: implement_( new FileReader(name) )
		{
		}
		StreamWriter::StreamWriter(Binary& bin)
		: implement_( new BinaryWriter(bin) )
		{
		}
		StreamReader::StreamReader(Binary const& bin)
		: implement_( new BinaryReader(bin) )
		{
		}

		uint8_t StreamReader::read()
		{
			if( eof() ) throw std::runtime_error("is eof");
			return implement_->read();
		}
		uint StreamReader::read(uint8_t* data, uint size)
		{
			bool res = ( tell()+size ) <= this->size();
			if( !res ) throw std::runtime_error("reached EOF");
			return implement_->read(data, size);
		}
		uint StreamReader::read(Binary& b)
		{
			bool res = ( tell()+b.size() ) <= this->size();
			if( !res ) throw std::runtime_error("reached EOF");
			return implement_->read( b.pointer(), b.size() );
		}

		void StreamWriter::write(uint8_t data)
		{
			if( this->size() <= tell() ) resize( tell() + sizeof(uint8_t) );
			implement_->write(data);
		}
		uint StreamWriter::write(uint8_t const* data, uint size)
		{
			if( this->size() < ( tell()+size ) ) resize( tell()+size );
			return implement_->write(data, size);
		}
		uint StreamWriter::write(Binary const& b)
		{
			if( this->size() < ( tell()+b.size() ) ) resize( tell()+b.size() );
			return implement_->write( b.pointer(), b.size() );
		}

		uint StreamReader::getBER()
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
		uint StreamWriter::setBER(uint num)
		{
			// BER output buffer
			uint8_t buff[ ( sizeof(num) * CHAR_BIT ) / BER_BIT + 1];
			uint size = getBERSize(num), index = size;
			// uint numBack = num;
		// set data
			buff[--index] = num & BER_MASK; // BER terminator
			num >>= BER_BIT;
			while(num) {
				buff[--index] = (num & BER_MASK) | BER_SIGN;
				num >>= BER_BIT;
			}
/*
			clog << numBack << " = " << size << " :";
			clog << std::setfill('0') << std::hex;
			for(uint i = 0; i < size; i++) clog << " " << std::setw(2) << (buff[i] & 0xff);
			clog << std::setfill(' ') << std::dec;
			clog << ";" << endl;
 */
		// write data
			write(buff, size);
			return size;
		}
		bool StreamReader::checkHeader(RPG2kString const& header)
		{
			this->seekFromSet();
			#if RPG2K_DEBUG
				Binary buf;
				this->get(buf);
				// cout << buf.size() << endl;
				// cout << static_cast<RPG2kString>(buf) << endl;
				return static_cast< RPG2kString >(buf) == header;
			#else
				Binary buf;
				return static_cast< RPG2kString >( this->get(buf) ) == header;
			#endif
		}

		FileInterface::FileInterface(SystemString const& filename, char const* mode)
		: filePointer_( std::fopen( filename.c_str(), mode ) ), name_(filename)
		{
			rpg2k_assert(filePointer_);

			size_ = seekFromEnd(0);
			seekFromSet(0);
		}
		#define PP_seekDefine(name1, name2) \
			uint FileInterface::seekFrom##name1(int val) \
			{ \
				bool res = fseek(filePointer_, val, SEEK_##name2) == 0; rpg2k_assert(res); \
				return tell(); \
			}
		PP_seekDefine(Set, SET)
		PP_seekDefine(Cur, CUR)
		PP_seekDefine(End, END)
		#undef seekDefine
		FileReader::FileReader(SystemString const& name)
		: FileInterface(name, "rb")
		{
		}
		FileReader::~FileReader()
		{
		}
		FileWriter::FileWriter(SystemString const& name)
		: FileInterface(name, "w+b")
		{
		}
		FileWriter::~FileWriter()
		{
		}
		uint FileReader::read(uint8_t* data, uint size)
		{
			return std::fread( data, sizeof(uint8_t), size, getFilePointer() );
		}
		uint8_t FileReader::read()
		{
			int ret = std::fgetc( getFilePointer() ); rpg2k_assert( ret != EOF );
			return ret;
		}
		uint FileWriter::write(uint8_t const* data, uint size)
		{
			return std::fwrite( data, sizeof(uint8_t), size, getFilePointer() );
		}
		void FileWriter::write(uint8_t data)
		{
			bool res = std::fputc( int(data), getFilePointer() ) != EOF; rpg2k_assert(res);
		}

		FileInterface::~FileInterface()
		{
			bool res = std::fclose(filePointer_) != EOF; rpg2k_assert(res);
		}

		BinaryReader::BinaryReader(Binary const& bin)
		: seek_(0), binary_(bin)
		{
		}
		BinaryWriter::BinaryWriter(Binary& bin)
		: seek_(0), binary_(bin)
		{
		}
		BinaryWriter::~BinaryWriter()
		{
			resize( tell() );
		}

		#define PP_binarySeek(type) \
			uint Binary##type::seekFromSet(int val) \
			{ \
				if( val > int(size()) ) getSeek() = size(); \
				else getSeek() = val; \
				 \
				return getSeek(); \
			} \
			uint Binary##type::seekFromCur(int val) \
			{ \
				if( getSeek() + val > int(size()) ) getSeek() = size(); \
				else getSeek() += val; \
				 \
				return getSeek(); \
			} \
			uint Binary##type::seekFromEnd(int val) \
			{ \
				int len = size(); \
				 \
				if( (len - val) > len ) getSeek() = 0; \
				else getSeek() = len - val - 1; \
				 \
				return getSeek(); \
			}
		PP_binarySeek(Reader)
		PP_binarySeek(Writer)
		#undef PP_binarySeek

		uint8_t BinaryReader::read()
		{
			return getBinary()[getSeek()++];
		}
		uint BinaryReader::read(uint8_t* data, uint size)
		{
			uint& seek = getSeek();

			memcpy( data, getBinary().pointer(seek), size );
			seek += size;

			return size;
		}
		void BinaryWriter::write(uint8_t data)
		{
			getBinary().pointer()[getSeek()++] = data;
		}
		uint BinaryWriter::write(uint8_t const* data, uint size)
		{
			uint& seek = getSeek();

			memcpy( getBinary().pointer(seek), data, size );
			seek += size;

			return size;
		}
	} // namespace structure
} // namespace rpg2k
