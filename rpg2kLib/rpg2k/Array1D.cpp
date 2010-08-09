#include "Array1D.hpp"
#include "Array2D.hpp"
#include "Element.hpp"
#include "Debug.hpp"


namespace rpg2k
{
	namespace structure
	{
		static unsigned int const BIG_DATA_SIZE = 1024;

		bool Array1D::createAt(uint pos)
		{
			std::map< uint, Binary >::iterator it = binBuf_.find(pos);
			if( it == binBuf_.end() ) return false;

			if( isArray2D() ) data_.addPointer( pos, Element::create(getOwner(), getIndex(), pos, it->second) );
			else data_.addPointer( pos, Element::create(*this, pos, it->second) );
			binBuf_.erase(it);

			return true;
		}

		Array1D::Array1D(Array1D const& src)
		: /* data_(src.data_), */ binBuf_(src.binBuf_)
		, arrayDefine_(src.arrayDefine_), this_(src.this_)
		, exists_(src.exists_), owner_(src.owner_), index_(src.index_)
		{
			for(Iterator it = src.begin(); it != src.end(); ++it) {
				if( !it.second().exists() ) continue;

				Binary bin = it.second().serialize();
				if( bin.size() >= BIG_DATA_SIZE ) binBuf_.insert( std::make_pair( it.first(), bin ) );
				else if( src.isArray2D() ) {
					data_.addPointer( it.first(), Element::create(src.getOwner(), src.getIndex(), it.first(), bin) );
				} else data_.addPointer( it.first(), Element::create(*this, it.first(), bin) );
			}
		}

		Array1D::Array1D(ArrayDefine info)
		: arrayDefine_(info), this_(NULL)
		{
			exists_ = false;
		}
		Array1D::Array1D(ArrayDefine info, StreamReader& s)
		: arrayDefine_(info), this_(NULL)
		{
			init(s);
		}
		Array1D::Array1D(ArrayDefine info, Binary const& b)
		: arrayDefine_(info), this_(NULL)
		{
			StreamReader s(b);
			init(s);
		}

		Array1D::Array1D(Element& e, Descriptor const& info)
		: arrayDefine_( info.getArrayDefine() ), this_(&e)
		{
			owner_ = NULL;
			exists_ = false;
		}
		Array1D::Array1D(Element& e, Descriptor const& info, StreamReader& s)
		: arrayDefine_( info.getArrayDefine() ), this_(&e)
		{
			init(s);
		}
		Array1D::Array1D(Element& e, Descriptor const& info, Binary const& b)
		: arrayDefine_( info.getArrayDefine() ), this_(&e)
		{
			StreamReader s(b);
			init(s);
		}
		Array1D::Array1D(Array2D& owner, uint index)
		: arrayDefine_( owner.getArrayDefine() ), this_(NULL)
		{
			exists_ = false;
			index_ = index;
			owner_ = &owner;
		}
		Array1D::Array1D(Array2D& owner, uint index, StreamReader& s)
		: arrayDefine_( owner.getArrayDefine() ), this_(NULL)
		{
			exists_ = true;
			index_ = index;
			owner_ = &owner;

			Binary bin;

			while(true) {
				uint index2 = s.getBER();

				if(index2 == ARRAY_1D_END) break;

				s.get(bin);
				if( bin.size() >= BIG_DATA_SIZE ) binBuf_.insert( std::make_pair(index2, bin) );
				else data_.addPointer( index2, Element::create(owner, index, index2, bin) );
			}
		}
		void Array1D::init(StreamReader& s)
		{
			owner_ = NULL;
			exists_ = true;

			Binary bin;

			while(true) {
				uint index = s.getBER();

				if(index == ARRAY_1D_END) break;

				s.get(bin);
				if( bin.size() >= BIG_DATA_SIZE ) binBuf_.insert( std::make_pair(index, bin) );
				else data_.addPointer( index, Element::create(*this, index, bin) );

				if( !toElement().hasOwner() && s.eof() ) return;
			}

			rpg2k_assert( s.eof() );
		}

		Array1D::~Array1D()
		{
		}

		bool Array1D::isElement() const
		{
			return (this_ != NULL) || ( isArray2D() && owner_->isElement() );
		}

		Element& Array1D::toElement() const
		{
			rpg2k_assert( isElement() );

			if( isArray2D() ) return owner_->toElement();
			else return *this_;
		}

		Array1D& Array1D::operator =(Array1D const& src)
		{
			exists_ = src.exists_;
			owner_ = src.owner_;
			// index_ = src.index_;

			data_ = src.data_;
			return *this;
		}

		// #define TRACE_ALL

		Element& Array1D::operator [](uint index)
		{
			if( data_.exists(index) || createAt(index) ) return data_[index];
			else {
				if( isArray2D() ) data_.addPointer( index, Element::create(*owner_, index_, index) );
				else data_.addPointer( index, Element::create(*this, index) );
				return data_[index];
			}
		}

		uint Array1D::getExistence() const
		{
			uint ret = 0;
			for(Iterator it = begin(); it != end(); ++it) if( it.second().exists() ) ret++;
			return ret;
		}
		uint Array1D::serializedSize() const
		{
			uint ret = 0;

			for(Iterator it = begin(); it != end(); ++it) {
				if( !it.second().exists() ) continue;

				uint index = it.first();
				ret += getBERSize(index);
				uint size = it.second().serializedSize();
				ret += getBERSize(size);
				ret += size;
			}
			for(std::map< uint, Binary >::const_iterator it = binBuf_.begin(); it != binBuf_.end(); ++it) {
				uint index = it->first;
				ret += getBERSize(index);
				uint size = it->second.size();
				ret += getBERSize(size);
				ret += size;
			}

			if( toElement().hasOwner() ) ret += getBERSize(ARRAY_1D_END);

			return ret;
		}
		void Array1D::serialize(StreamWriter& s) const
		{
			s.add( serializedSize() );

			for(Iterator it = begin(); it != end(); ++it) {
				if( !it.second().exists() ) continue;

				s.setBER( it.first() );
				s.setBER( it.second().serializedSize() );
				it.second().serialize(s);
			}
			for(std::map< uint, Binary >::const_iterator it = binBuf_.begin(); it != binBuf_.end(); ++it) {
				s.setBER( it->first );
				s.setBER( it->second.size() );
				s.write(it->second);
			}

			if( toElement().hasOwner() ) s.setBER(ARRAY_1D_END);
		}

		uint const& Array1D::getIndex() const { rpg2k_assert( isArray2D() ); return index_; }
		bool Array1D::exists() const { rpg2k_assert( isArray2D() ); return exists_; }
		Array2D& Array1D::getOwner() { rpg2k_assert( isArray2D() ); return *owner_; }
		Array2D const& Array1D::getOwner() const { rpg2k_assert( isArray2D() ); return *owner_; }

		void Array1D::substantiate()
		{
			rpg2k_assert( isArray2D() );

			toElement().substantiate();
			exists_ = true;
		}
		bool Array1D::exists(uint index) const
		{
			return data_.exists(index) ? data_[index].exists() : ( binBuf_.find(index) != binBuf_.end() );
		}

		std::auto_ptr<Array1D> Array1D::copy(Array1D const& src)
		{
			return std::auto_ptr<Array1D>( new Array1D(src) );
		}
	} // namespace structure
} // namespace rpg2k
