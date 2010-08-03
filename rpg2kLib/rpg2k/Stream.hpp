#ifndef _INC__RPG2K__FILE_HPP
#define _INC__RPG2K__FILE_HPP

#include "Structure.hpp"

#include <cstdio>
#include <string>


namespace rpg2k
{
	namespace structure
	{
		class StreamInterface
		{
		protected:
			StreamInterface() {}
			StreamInterface(StreamInterface const& i) {}
		public:
			virtual ~StreamInterface() {}

			virtual SystemString const& name() const { throw std::runtime_error("Unimplemented"); }

			virtual uint seekFromSet(uint val) = 0;
			virtual uint seekFromCur(uint val) = 0;
			virtual uint seekFromEnd(uint val) = 0;

			virtual uint size() const = 0;

			virtual uint tell() const { throw std::runtime_error("Unimplemented"); }

			virtual uint8_t read() { throw std::runtime_error("Unimplemented"); }
			virtual uint read(uint8_t* data, uint size) { throw std::runtime_error("Unimplemented"); }

			virtual void write(uint8_t data) { throw std::runtime_error("Unimplemented"); }
			virtual uint write(uint8_t const* data, uint size) { throw std::runtime_error("Unimplemented"); }

			virtual void resize(uint size) { throw std::runtime_error("Unimplemented"); }
		};

		class FileInterface : public StreamInterface
		{
		private:
			FILE* filePointer_;
			SystemString name_;
			uint size_;
		protected:
			void setSize(uint val) { size_ = val; }
			FILE* getFilePointer() { return filePointer_; }
			FileInterface(SystemString const& filename, char const* mode);
		public:
			virtual ~FileInterface();

			SystemString const& name() const { return name_; }

			uint seekFromSet(uint val);
			uint seekFromCur(uint val);
			uint seekFromEnd(uint val);

			uint size() const { return size_; }

			uint tell() const { return std::ftell(filePointer_); }
		};
		class FileReader : public FileInterface
		{
		public:
			using FileInterface::size;
			using FileInterface::name;

			FileReader(SystemString const& name);
			virtual ~FileReader();

			uint8_t read();
			uint read(uint8_t* data, uint size);
		};
		class FileWriter : public FileInterface
		{
		public:
			FileWriter(SystemString const& name);
			virtual ~FileWriter();

			using FileInterface::size;

			void resize(uint size) { setSize(size); }

			void write(uint8_t data);
			uint write(uint8_t const* data, uint size);
		};

		class BinaryWriter : public StreamInterface
		{
		private:
			uint seek_;
			Binary& binary_;
		protected:
			uint& getSeek() { return seek_; }
			Binary& getBinary() { return binary_; }
		public:
			 BinaryWriter(Binary& bin);
			 ~BinaryWriter();

			uint tell() const { return seek_; }
			uint size() const { return binary_.size(); }
			void resize(uint size) { getBinary().resize(size); }

			void write(uint8_t data);
			uint write(uint8_t const* data, uint size);

			uint seekFromSet(uint val);
			uint seekFromCur(uint val);
			uint seekFromEnd(uint val);
		};
		class BinaryReader : public StreamInterface
		{
		private:
			uint seek_;
			Binary const& binary_;
		protected:
			uint& getSeek() { return seek_; }
			Binary const& getBinary() { return binary_; }
		public:
			 BinaryReader(Binary const& bin);

			uint tell() const { return seek_; }
			uint size() const { return binary_.size(); }

			uint8_t read();
			uint read(uint8_t* data, uint size);

			uint seekFromSet(uint val);
			uint seekFromCur(uint val);
			uint seekFromEnd(uint val);
		};

		class StreamReader
		{
		private:
			boost::shared_ptr< StreamInterface > implement_;
		protected:
			StreamReader();
			StreamReader(StreamReader const& s);
			StreamReader& operator =(StreamReader const& s);

			StreamInterface& getImplement() { return *implement_; }
		public:
			StreamReader(boost::shared_ptr< StreamInterface > const& imp);
			// virtual ~StreamReader();

			StreamReader(Binary const& bin);
			StreamReader(SystemString const& str);

			void close();

			uint size() const { return implement_->size(); }

			SystemString const& name() const { return implement_->name(); }

			uint seekFromSet(uint val = 0) { return implement_->seekFromSet(val); }
			uint seekFromCur(uint val = 0) { return implement_->seekFromCur(val); }
			uint seekFromEnd(uint val = 0) { return implement_->seekFromEnd(val); }
			uint seek(uint pos = 0) { return seekFromSet(pos); }

			uint tell() const { return implement_->tell(); }

			bool eof() const { return tell() >= size(); }

			uint8_t read();
			uint read(uint8_t* data, uint size);
		/*
		 * read and copy to argument
		 * reading data size would be b.size()
		 */
			uint read(Binary& b);

			uint getBER();

			Binary& get(Binary& b) { b.resize( getBER() ); read(b); return b; }

			bool checkHeader(RPG2kString const& header);
		};

		class StreamWriter
		{
		private:
			boost::shared_ptr< StreamInterface > implement_;
		protected:
			StreamInterface& getImplement() { return *implement_; }

			StreamWriter();
			StreamWriter(StreamWriter const& s);
			StreamWriter& operator =(StreamWriter const& s);
		public:
			StreamWriter(boost::shared_ptr< StreamInterface > const& imp);
			// virtual ~StreamWriter();

			StreamWriter(Binary& bin);
			StreamWriter(SystemString const& str);

			void close();

			uint size() const { return implement_->size(); }
			void resize(uint size) { implement_->resize(size); }

			SystemString const& name() const { return implement_->name(); }

			uint seekFromSet(uint val = 0) { return implement_->seekFromSet(val); }
			uint seekFromCur(uint val = 0) { return implement_->seekFromCur(val); }
			uint seekFromEnd(uint val = 0) { return implement_->seekFromEnd(val); }
			uint seek(uint pos = 0) { return seekFromSet(pos); }

			uint tell() const { return implement_->tell(); }

			void add(uint size) { implement_->resize( tell() + ++size ); }

			void write(uint8_t data);
			uint write(uint8_t const* data, uint size);
			uint write(Binary const& b);

			uint setBER(uint num);
			void set(Binary const& b) { setBER( b.size() ); write(b); }

			void setHeader(RPG2kString const& header) { seekFromSet(); set( Binary(header) ); }
		};

		template< class T >
		Binary serialize(T const& src)
		{
			Binary b( src.serializedSize() );
			StreamWriter s(b);

			src.serialize(s);

			return b;
		}
	} // namespace structure
} // namespace rpg2k

#endif
