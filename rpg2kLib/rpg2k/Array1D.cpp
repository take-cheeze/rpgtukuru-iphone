#include "Array1D.hpp"
#include "Array2D.hpp"
#include "Debug.hpp"
#include "Element.hpp"
#include "Stream.hpp"

#include <algorithm>


namespace rpg2k
{
	namespace structure
	{
		namespace
		{
			// doesn't extract if the Element of Array1D is bigger than the next value
			#if RPG2K_DEBUG // if debuging ALL Element will be extracted
				static std::size_t const BIG_DATA_SIZE = ~0;
			#else
				static std::size_t const BIG_DATA_SIZE = 512;
			#endif
		} // namespace

		bool Array1D::createAt(unsigned const pos)
		{
			std::map<unsigned, Binary>::iterator it = binBuf_.find(pos);
			if( it == binBuf_.end() ) return false;

			if( isArray2D() ) insert( pos, std::auto_ptr<Element>( new Element(owner(), index(), pos, it->second) ) );
			else insert( pos, std::auto_ptr<Element>( new Element(*this, pos, it->second) ) );
			binBuf_.erase(it);

			return true;
		}

		Array1D::Array1D(Array1D const& src)
		: BaseOfArray1D(src)
		, binBuf_(src.binBuf_)
		, arrayDefine_(src.arrayDefine_), this_(src.this_)
		, exists_(src.exists_), owner_(src.owner_), index_(src.index_)
		{
			this->clear();
			for(const_iterator it = src.begin(); it != src.end(); ++it) {
				if( !it->second->exists() ) continue;

				Binary bin = it->second->serialize();

				if( bin.size() >= BIG_DATA_SIZE ) {
					binBuf_.insert( std::make_pair( it->first, bin ) );
				} else if( src.isArray2D() ) {
					insert( it->first, std::auto_ptr<Element>( new Element(
						src.owner(), src.index(), it->first, bin) ) );
				} else {
					insert( it->first, std::auto_ptr<Element>(
						new Element(*this, it->first, bin) ) );
				}
			}
		}

		Array1D::Array1D(ArrayDefine info)
		: arrayDefine_(info), this_(NULL), owner_(NULL), index_(-1)
		{
			exists_ = false;
		}
		Array1D::Array1D(ArrayDefine info, StreamReader& s)
		: arrayDefine_(info), this_(NULL), owner_(NULL), index_(-1)
		{
			init(s);
		}
		Array1D::Array1D(ArrayDefine info, Binary const& b)
		: arrayDefine_(info), this_(NULL), owner_(NULL), index_(-1)
		{
			StreamReader s( std::auto_ptr<StreamInterface>( new BinaryReaderNoCopy(b) ) );
			init(s);
		}

		Array1D::Array1D(Element& e)
		: arrayDefine_( e.descriptor().arrayDefine() ), this_(&e)
		, owner_(NULL), index_(-1)
		{
			exists_ = false;
		}
		Array1D::Array1D(Element& e, StreamReader& s)
		: arrayDefine_( e.descriptor().arrayDefine() ), this_(&e)
		, owner_(NULL), index_(-1)
		{
			init(s);
		}
		Array1D::Array1D(Element& e, Binary const& b)
		: arrayDefine_( e.descriptor().arrayDefine() ), this_(&e)
		, owner_(NULL), index_(-1)
		{
			StreamReader s( std::auto_ptr<StreamInterface>( new BinaryReaderNoCopy(b) ) );
			init(s);
		}
		Array1D::Array1D(Array2D& owner, unsigned index)
		: arrayDefine_( owner.arrayDefine() ), this_(NULL)
		, owner_(&owner), index_(index)
		{
			exists_ = false;
		}
		Array1D::Array1D(Array2D& owner, unsigned index, StreamReader& s)
		: arrayDefine_( owner.arrayDefine() ), this_(NULL)
		, owner_(&owner), index_(index)
		{
			exists_ = true;

			Binary bin;

			while(true) {
				unsigned index2 = s.ber();

				if(index2 == ARRAY_1D_END) break;

				s.get(bin);
				if( bin.size() >= BIG_DATA_SIZE ) binBuf_.insert( std::make_pair(index2, bin) );
				else insert( index2, std::auto_ptr<Element>( new Element(owner, index, index2, bin) ) );
			}
		}
		void Array1D::init(StreamReader& s)
		{
			exists_ = true;

			Binary bin;

			while(true) {
				unsigned index = s.ber();

				if(index == ARRAY_1D_END) break;

				s.get(bin);
				if( bin.size() >= BIG_DATA_SIZE ) binBuf_.insert( std::make_pair(index, bin) );
				else insert( index, std::auto_ptr<Element>( new Element(*this, index, bin) ) );

				if( !toElement().hasOwner() && s.eof() ) return;
			}

			rpg2k_analyze_assert( s.eof() );
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

		Array1D const& Array1D::operator =(Array1D const& src)
		{
			BaseOfArray1D::operator =(src);
			exists_ = src.exists_;

			return *this;
		}

		Element& Array1D::operator [](unsigned const index)
		{
			iterator it = find(index);
			if( it != end() ) {
				return *it->second;
			} else if( createAt(index) ) {
				return *this->find(index)->second;
			} else {
				if( isArray2D() ) {
					return *insert( index, std::auto_ptr<Element>(
						new Element(*owner_, index_, index) ) ).first->second;
				} else {
					return *insert( index, std::auto_ptr<Element>(
						new Element(*this, index) ) ).first->second;
				}
			}
		}
		Element const& Array1D::operator [](unsigned const index) const
		{
			return const_cast<Array1D&>(*this)[index];
		}

		unsigned Array1D::count() const
		{
			unsigned ret = 0;
			for(const_iterator it = begin(); it != end(); ++it) {
				if( it->second->exists() ) ret++;
			}
			return ret;
		}
		unsigned Array1D::serializedSize() const
		{
			unsigned ret = 0;

			for(const_iterator it = begin(); it != end(); ++it) {
				if( !it->second->exists() ) continue;

				ret += berSize(it->first);
				unsigned const size = it->second->serializedSize();
				ret += berSize(size);
				ret += size;
			}
			for(std::map<unsigned, Binary>::const_iterator it = binBuf_.begin(); it != binBuf_.end(); ++it) {
				ret += berSize(it->first);
				unsigned const size = it->second.size();
				ret += berSize(size);
				ret += size;
			}

			if( toElement().hasOwner() ) ret += berSize(ARRAY_1D_END);

			return ret;
		}
		void Array1D::serialize(StreamWriter& s) const
		{
			for(const_iterator it = begin(); it != end(); ++it) {
				if( !it->second->exists() ) continue;

				s.setBER( it->first );
				s.setBER( it->second->serializedSize() );
				it->second->serialize(s);
			}
			for(std::map<unsigned, Binary>::const_iterator it = binBuf_.begin(); it != binBuf_.end(); ++it) {
				s.setBER( it->first );
				s.setBER( it->second.size() );
				s.write(it->second);
			}

			if( toElement().hasOwner() ) s.setBER(ARRAY_1D_END);
		}

		unsigned const& Array1D::index() const { rpg2k_assert( isArray2D() ); return index_; }
		bool Array1D::exists() const { rpg2k_assert( isArray2D() ); return exists_; }
		Array2D& Array1D::owner() { rpg2k_assert( isArray2D() ); return *owner_; }
		Array2D const& Array1D::owner() const { rpg2k_assert( isArray2D() ); return *owner_; }

		void Array1D::substantiate()
		{
			rpg2k_assert( isArray2D() );

			toElement().substantiate();
			exists_ = true;
		}
		bool Array1D::exists(unsigned index) const
		{
			const_iterator it = find(index);
			return ( ( it != end() ) && it->second->exists() ) ||
				( binBuf_.find(index) != binBuf_.end() );
		}
	} // namespace structure
} // namespace rpg2k
