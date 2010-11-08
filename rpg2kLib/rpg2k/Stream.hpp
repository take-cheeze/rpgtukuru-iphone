#ifndef _INC__RPG2K__FILE_HPP
#define _INC__RPG2K__FILE_HPP

#include "Structure.hpp"

#include <cstdio>

#include <memory>
#include <stdexcept>
#include <string>

#include <boost/noncopyable.hpp>


namespace rpg2k
{
	namespace structure
	{
		class StreamReader;
		class StreamWriter;

		class StreamInterface : boost::noncopyable
		{
		public:
			virtual ~StreamInterface() {}

			virtual SystemString const& name() const { throw std::runtime_error("Unimplemented"); }

			virtual unsigned seekFromSet(int val) = 0;
			virtual unsigned seekFromCur(int val) = 0;
			virtual unsigned seekFromEnd(int val) = 0;

			virtual unsigned size() const = 0;

			virtual unsigned tell() const { throw std::runtime_error("Unimplemented"); }

			virtual uint8_t read() { throw std::runtime_error("Unimplemented"); }
			virtual unsigned read(uint8_t* data, unsigned size) { throw std::runtime_error("Unimplemented"); }

			virtual void write(uint8_t data) { throw std::runtime_error("Unimplemented"); }
			virtual unsigned write(uint8_t const* data, unsigned size) { throw std::runtime_error("Unimplemented"); }

			virtual void resize(unsigned size) { throw std::runtime_error("Unimplemented"); }
		};

		class FileInterface : public StreamInterface
		{
		private:
			FILE* filePointer_;
			SystemString name_;
			unsigned size_;

			virtual SystemString const& name() const { return name_; }

			virtual unsigned seekFromSet(int val);
			virtual unsigned seekFromCur(int val);
			virtual unsigned seekFromEnd(int val);

			virtual unsigned size() const { return size_; }

			virtual unsigned tell() const { return std::ftell(filePointer_); }
		protected:
			void setSize(unsigned val) { size_ = val; }
			FILE* filePointer() { return filePointer_; }

			FileInterface(SystemString const& filename, char const* mode);
			virtual ~FileInterface();
		};
		class FileReader : public FileInterface
		{
		private:
			uint8_t read();
			unsigned read(uint8_t* data, unsigned size);
		public:
			FileReader(SystemString const& name);
		};
		class FileWriter : public FileInterface
		{
		private:
			void resize(unsigned size) { setSize(size); }

			void write(uint8_t data);
			unsigned write(uint8_t const* data, unsigned size);
		public:
			FileWriter(SystemString const& name);
		};

		class BinaryWriter : public StreamInterface
		{
		private:
			unsigned seek_;
			Binary binary_;

			virtual ~BinaryWriter();

			virtual unsigned tell() const { return seek_; }
			virtual unsigned size() const { return binary_.size(); }
			virtual void resize(unsigned size) { binary().resize(size); }

			virtual void write(uint8_t data);
			virtual unsigned write(uint8_t const* data, unsigned size);

			virtual unsigned seekFromSet(int val);
			virtual unsigned seekFromCur(int val);
			virtual unsigned seekFromEnd(int val);
		protected:
			unsigned& seekPos() { return seek_; }
			Binary& binary() { return binary_; }
		public:
			BinaryWriter(Binary const& bin);
			Binary bin() const { return binary_; }
		};
		class BinaryReader : public StreamInterface
		{
		private:
			unsigned seek_;
			Binary const binary_;

			virtual unsigned tell() const { return seek_; }
			virtual unsigned size() const { return binary_.size(); }

			virtual uint8_t read();
			virtual unsigned read(uint8_t* data, unsigned size);

			virtual unsigned seekFromSet(int val);
			virtual unsigned seekFromCur(int val);
			virtual unsigned seekFromEnd(int val);
		protected:
			unsigned& seekPos() { return seek_; }
			Binary const& binary() { return binary_; }
		public:
			BinaryReader(Binary const& bin);
		};
		class BinaryReaderNoCopy : public StreamInterface
		{
		private:
			unsigned seek_;
			Binary const& binary_;

			virtual unsigned tell() const { return seek_; }
			virtual unsigned size() const { return binary_.size(); }

			virtual uint8_t read();
			virtual unsigned read(uint8_t* data, unsigned size);

			virtual unsigned seekFromSet(int val);
			virtual unsigned seekFromCur(int val);
			virtual unsigned seekFromEnd(int val);
		protected:
			unsigned& seekPos() { return seek_; }
			Binary const& binary() { return binary_; }
		public:
			BinaryReaderNoCopy(Binary const& bin);
		};

		class StreamReader
		{
		private:
			std::auto_ptr<StreamInterface> implement_;
		protected:
			StreamReader();
			StreamReader(StreamReader const& s);
			StreamReader const& operator =(StreamReader const& s);

			StreamInterface& implement() { return *implement_; }
		public:
			StreamReader(std::auto_ptr<StreamInterface> imp);

			StreamReader(Binary const& bin);
			StreamReader(SystemString const& str);

			void close();

			unsigned size() const { return implement_->size(); }

			SystemString const& name() const { return implement_->name(); }

			unsigned seekFromSet(int val = 0) { return implement_->seekFromSet(val); }
			unsigned seekFromCur(int val = 0) { return implement_->seekFromCur(val); }
			unsigned seekFromEnd(int val = 0) { return implement_->seekFromEnd(val); }
			unsigned seek(unsigned pos = 0) { return seekFromSet(pos); }

			unsigned tell() const { return implement_->tell(); }

			bool eof() const { return tell() >= size(); }

			uint8_t read();
			unsigned read(uint8_t* data, unsigned size);
		/*
		 * read and copy to argument
		 * reading data size would be b.size()
		 */
			unsigned read(Binary& b);

			unsigned ber();

			Binary& get(Binary& b) { b.resize( ber() ); read(b); return b; }

			bool checkHeader(RPG2kString const& header);
		};

		class StreamWriter
		{
		private:
			std::auto_ptr<StreamInterface> implement_;

			StreamWriter();
			StreamWriter(StreamWriter const& s);
			StreamWriter const& operator =(StreamWriter const& s);
		protected:
			StreamInterface& implement() { return *implement_; }
		public:
			StreamWriter(std::auto_ptr<StreamInterface> imp);

			StreamWriter(Binary& bin);
			StreamWriter(SystemString const& str);

			void close();

			unsigned size() const { return implement_->size(); }
			void resize(unsigned size) { implement_->resize(size); }

			SystemString const& name() const { return implement_->name(); }

			unsigned seekFromSet(int val = 0) { return implement_->seekFromSet(val); }
			unsigned seekFromCur(int val = 0) { return implement_->seekFromCur(val); }
			unsigned seekFromEnd(int val = 0) { return implement_->seekFromEnd(val); }
			unsigned seek(unsigned pos = 0) { return seekFromSet(pos); }

			unsigned tell() const { return implement_->tell(); }

			void add(unsigned size) { implement_->resize( tell() + ++size ); }

			void write(uint8_t data);
			unsigned write(uint8_t const* data, unsigned size);
			unsigned write(Binary const& b);

			unsigned setBER(unsigned num);
			void set(Binary const& b) { setBER( b.size() ); write(b); }

			void setHeader(RPG2kString const& header) { seekFromSet(); set( Binary(header) ); }
		};

		template<class T>
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
