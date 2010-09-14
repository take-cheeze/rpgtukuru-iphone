#include "Array1D.hpp"
#include "Array2D.hpp"
#include "Debug.hpp"
#include "Element.hpp"


namespace rpg2k
{
	namespace structure
	{
		Array2D::Array2D(Array2D const& src)
		: /* data_(src.data_), */ arrayDefine_(src.arrayDefine_)
		, this_(src.this_)
		{
			for(Iterator it = src.begin(); it != src.end(); ++it) {
				StreamReader stream( structure::serialize( it.second() ) );
				data_.add( it.first(), *this, it.first(), stream );
			}
		}

		Array2D::Array2D(ArrayDefine info)
		: arrayDefine_(info), this_(NULL)
		{
		}
		Array2D::Array2D(ArrayDefine info, StreamReader& s)
		: arrayDefine_(info), this_(NULL)
		{
			init(s);
		}
		Array2D::Array2D(ArrayDefine info, Binary const& b)
		: arrayDefine_(info), this_(NULL)
		{
			StreamReader s(b);

			if( isInvalidArray2D(b) ) return; // s.seek(PARTICULAR_DATA_SIZE);
			init(s);
		}

		Array2D::Array2D(Element& e, Descriptor const& info)
		: arrayDefine_( info.getArrayDefine() ), this_(&e)
		{
		}
		Array2D::Array2D(Element& e, Descriptor const& info, StreamReader& s)
		: arrayDefine_( info.getArrayDefine() ), this_(&e)
		{
			init(s);
		}
		Array2D::Array2D(Element& e, Descriptor const& info, Binary const& b)
		: arrayDefine_( info.getArrayDefine() ), this_(&e)
		{
			StreamReader s(b);

			if( isInvalidArray2D(b) ) return; // s.seek(PARTICULAR_DATA_SIZE);
			init(s);
		}
		void Array2D::init(StreamReader& s)
		{
			for(uint i = 0, length = s.getBER(); i < length; i++) {
				uint index = s.getBER();
				data_.add(index, *this, index, s);
			}

			if( toElement().hasOwner() ) rpg2k_assert( s.eof() );
		}

	/*
	 *  Checking if the input Binary is particular Array2D.
	 *  Particular Array2D is a Binary data that starts with 512 byte data of
	 * all value that is 0xff.
	 *  Notice: Particular Array2D's size is not 512 byte. I misunderstood it.
	 *  http://twitter.com/rgssws4m told me about this case.
	 */
		bool Array2D::isInvalidArray2D(Binary const& b)
		{
			static uint const PARTICULAR_DATA_SIZE = 512;
		// check the data size
			if( b.size() < PARTICULAR_DATA_SIZE ) return false;
		// check the data inside Binary
			for(uint i = 0; i < PARTICULAR_DATA_SIZE; i++) if(b[i] != 0xff) return false;

			debug::Tracer::printBinary(b, clog);
		// return true if it is particular Array2D
			return true;
		}

		Element& Array2D::toElement() const
		{
			rpg2k_assert( isElement() );
			return *this_;
		}

		Array2D& Array2D::operator =(Array2D const& src)
		{
			data_ = src.data_;
			return *this;
		}

		Array1D& Array2D::operator [](uint index)
		{
			if( data_.exists(index) ) return data_[index];
			else {
				data_.add(index, *this, index);
				return data_[index];
			}
		}

		uint Array2D::getExistence() const
		{
			uint ret = 0;
			for(Iterator it = begin(); it != end(); ++it) {
				if( it.second().exists() ) ret++;
			}
			return ret;
		}
		uint Array2D::serializedSize() const
		{
			uint ret = 0;

			ret += getBERSize( getExistence() );
			for(Iterator it = begin(); it != end(); ++it) {
				if( !it.second().exists() ) continue;

				ret += getBERSize( it.first() );
				ret += it.second().serializedSize();
			}

			return ret;
		}
		void Array2D::serialize(StreamWriter& s) const
		{
			s.setBER( getExistence() );
			for(Iterator it = begin(); it != end(); ++it) {
				if( !it.second().exists() ) continue;

				s.setBER( it.first() );
				it.second().serialize(s);
			}
		}

		bool Array2D::exists(uint index) const
		{
			return data_.exists(index) ? data_[index].exists() : false;
		}
		bool Array2D::exists(uint index1, uint index2) const
		{
			return data_.exists(index1) ? data_[index1].exists(index2) : false;
		}
	} // namespace structure
} // namespace rpg2k
