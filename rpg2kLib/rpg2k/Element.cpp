#include "Array1D.hpp"
#include "Array1DWrapper.hpp"
#include "Array2D.hpp"
#include "Debug.hpp"
#include "Element.hpp"
#include "Event.hpp"
#include "Stream.hpp"

#include <stdexcept>


#if RPG2K_ANALYZE_AT_DESTRUCTOR
	#if RPG2K_ONLY_ANALYZE_NON_DEFINED_ELEMENT
		#define ANALYZE_SELF() \
			if( exists() && isDefined() ) debug::Tracer::printTrace(*this, true)
	#else
		#define ANALYZE_SELF() \
			if( exists() ) debug::Tracer::printTrace(*this, true)
	#endif
#else
	#define ANALYZE_SELF()
#endif

namespace rpg2k
{
	namespace structure
	{
		template<class T>
		bool checkSerialize(T const& result, Binary const& src)
		{
			Binary serialized = serialize(result);
			if(src != serialized) {
				debug::Tracer::printBinary(src, cerr);
				cerr << endl;
				debug::Tracer::printBinary(serialized, cerr);
				cerr << endl;

				return false;
			}
			return true;
		}

		BerEnum::BerEnum(StreamReader& s)
		{
			init(s);
		}
		BerEnum::BerEnum(Binary const& b)
		{
			StreamReader s( std::auto_ptr<StreamInterface>( new BinaryReaderNoCopy(b) ) );
			init(s);
			rpg2k_assert( s.eof() );
		}

		void BerEnum::init(StreamReader& s)
		{
			unsigned length = s.ber();

			resize(length+1);
			for(unsigned i = 0; i <= length; i++) (*this)[i] = s.ber();
		}

		unsigned BerEnum::serializedSize() const
		{
			unsigned ret = berSize( size() - 1 );
			for(unsigned i = 0; i < size(); i++) ret += berSize( (*this)[i] );

			return ret;
		}
		void BerEnum::serialize(StreamWriter& s) const
		{
			Binary b = ( std::vector<unsigned>& ) *this;
			s.setBER( b.size() - 1 );
			s.write(b);
		}
/*
		CharSetDir EventState::charSetDir() const
		{
			return (CharSetDir) (*this)[30 + talkDir()].get<int>();
		}
 */

		void Element::serialize(StreamWriter& s) const
		{
			if( isDefined() ) switch( descriptor_->type() ) {
				#define PP_enum(TYPE) \
					case ElementType::TYPE##_: \
						impl_.TYPE##_->serialize(s); \
						break;
				PP_rpg2kTypes(PP_enum)
				#undef PP_enum

				#define PP_enum(TYPE) \
					case ElementType::TYPE##_: { \
						Binary bin; \
						bin = *impl_.TYPE##_; \
						s.write(bin); \
					} break;
				PP_basicTypes(PP_enum)
				#undef PP_enum
			} else s.write(binData_);
		}
		unsigned Element::serializedSize() const
		{
			if( isDefined() ) switch( descriptor_->type() ) {
				#define PP_enum(TYPE) \
					case ElementType::TYPE##_: \
						return impl_.TYPE##_->serializedSize();
				PP_rpg2kTypes(PP_enum)
				#undef PP_enum

				#define PP_enum(TYPE) \
					case ElementType::TYPE##_: { \
						Binary bin; \
						bin = *impl_.TYPE##_; \
						return bin.size(); \
					}
				PP_basicTypes(PP_enum)
				#undef PP_enum

				default: rpg2k_assert(false); return 0;
			} else return binData_.size();
		}

		Element::Element(Element const& e)
		: descriptor_(e.descriptor_)
		, exists_(e.exists_), owner_(e.owner_)
		, index1_(e.index1_), index2_(e.index2_)
		{
			if( isDefined() ) switch( descriptor_->type() ) {
				#define PP_enum(TYPE) \
					case ElementType::TYPE##_: \
						impl_.TYPE##_ = new TYPE( *e.impl_.TYPE##_ ); \
						break;
				PP_basicTypes(PP_enum)
				PP_rpg2kTypes(PP_enum)
				#undef PP_enum
			}
		}

		void Element::init()
		{
			exists_ = false;

			if( descriptor_->hasDefault() ) switch( descriptor_->type() ) {
				#define PP_enum(TYPE) \
					case ElementType::TYPE##_: \
						impl_.TYPE##_ = new TYPE(*descriptor_); \
						break;
				PP_basicTypes(PP_enum)
				#undef PP_enum

				#define PP_enum(TYPE) \
					case ElementType::TYPE##_: \
						impl_.TYPE##_ = new TYPE(*this); \
						break;
				PP_enum(Array1D)
				PP_enum(Array2D)
				#undef PP_enum

				#define PP_enumNoDefault(TYPE) \
					case ElementType::TYPE##_: \
						impl_.TYPE##_ = new TYPE(); \
						break;
				PP_enumNoDefault(BerEnum)
				PP_enumNoDefault(Binary)
				PP_enumNoDefault(Event)

				default: rpg2k_analyze_assert(false); break;
			} else switch( descriptor_->type() ) {
				PP_basicTypes(PP_enumNoDefault)
				PP_enumNoDefault(BerEnum)
				PP_enumNoDefault(Binary)
				PP_enumNoDefault(Event)
				#undef PP_enumNoDefault

				default: rpg2k_analyze_assert(false); break;
			}
		}
		void Element::init(Binary const& b)
		{
			exists_ = true;

			if( isDefined() ) switch( descriptor_->type() ) {
				#define PP_enum(TYPE) \
					case ElementType::TYPE##_: \
						impl_.TYPE##_ = new TYPE(*this, b); \
						break;
				PP_enum(Array1D)
				PP_enum(Array2D)
				#undef PP_enum

				#define PP_enum(TYPE) \
					case ElementType::TYPE##_: \
						impl_.TYPE##_ = new TYPE(b); \
						break;
				PP_basicTypes(PP_enum)
				PP_enum(BerEnum)
				PP_enum(Binary)
				PP_enum(Event)
				#undef PP_enum
			} else {
				binData_ = b;
			}
		}
		void Element::init(StreamReader& s)
		{
			exists_ = true;

			switch( descriptor_->type() ) {
				#define PP_enum(TYPE) \
					case ElementType::TYPE##_: \
						impl_.TYPE##_ = new TYPE(*this, s); \
						break;
				PP_enum(Array1D)
				PP_enum(Array2D)
				#undef PP_enum

				case ElementType::BerEnum_:
					impl_.BerEnum_ = new BerEnum(s);
					break;
				default: rpg2k_analyze_assert(false); break;
			}
		}

		Element::Element()
		: descriptor_(NULL), owner_(NULL), index1_(-1), index2_(-1)
		{
			init();
		}
		Element::Element(Descriptor const& info)
		: descriptor_(&info), owner_(NULL), index1_(-1), index2_(-1)
		{
			init();
		}
		Element::Element(Descriptor const& info, Binary const& b)
		: descriptor_(&info), owner_(NULL), index1_(-1), index2_(-1)
		{
			#if RPG2K_CHECK_AT_CONSTRUCTOR
				rpg2k_analyze_assert( instance_.checkSerialize(b) );
			#endif

			init(b);
		}
		Element::Element(Descriptor const& info, StreamReader& s)
		: descriptor_(&info), owner_(NULL), index1_(-1), index2_(-1)
		{
			init(s);
		}

		Element::Element(Array1D const& owner, unsigned index)
		: descriptor_(
			owner.arrayDefine().find(index) != owner.arrayDefine().end()
				? owner.arrayDefine().find(index)->second : NULL )
		, owner_( &owner.toElement() )
		, index1_(index), index2_(-1)
		{
			init();
		}
		Element::Element(Array1D const& owner, unsigned index, Binary const& b)
		: descriptor_(
			owner.arrayDefine().find(index) != owner.arrayDefine().end()
				? owner.arrayDefine().find(index)->second : NULL )
		, owner_( &owner.toElement() )
		, index1_(index), index2_(-1)
		{
			#if RPG2K_CHECK_AT_CONSTRUCTOR
				rpg2k_analyze_assert( instance_.checkSerialize(b) );
			#endif

			init(b);
		}
		Element::Element(Array2D const& owner, unsigned index1, unsigned index2)
		: descriptor_(
			owner.arrayDefine().find(index2) != owner.arrayDefine().end()
				? owner.arrayDefine().find(index2)->second : NULL )
		, owner_ (&owner.toElement() )
		, index1_(index1), index2_(index2)
		{
			init();
		}
		Element::Element(Array2D const& owner, unsigned index1, unsigned index2, Binary const& b)
		: descriptor_(
			owner.arrayDefine().find(index2) != owner.arrayDefine().end()
				? owner.arrayDefine().find(index2)->second : NULL )
		, owner_ (&owner.toElement() )
		, index1_(index1), index2_(index2)
		{
			#if RPG2K_CHECK_AT_CONSTRUCTOR
				rpg2k_analyze_assert( instance_.checkSerialize(b) );
			#endif

			init(b);
		}

		Element::~Element()
		{
			ANALYZE_SELF();

			if( isDefined() ) switch( descriptor_->type() ) {
				#define PP_enum(TYPE) case ElementType::TYPE##_: delete impl_.TYPE##_; break;
				PP_basicTypes(PP_enum)
				PP_rpg2kTypes(PP_enum)
				#undef PP_enum
			}
		}

		Element const& Element::operator =(Element const& src)
		{
			if( isDefined() ) switch( descriptor_->type() ) {
				#define PP_enum(TYPE) \
					case ElementType::TYPE##_: \
						(*impl_.TYPE##_) = (*src.impl_.TYPE##_); \
						break;
				PP_basicTypes(PP_enum)
				PP_rpg2kTypes(PP_enum)
				#undef PP_enum
				default: rpg2k_assert(false); break;
			} else binData_ = src.binData_;

			return *this;
		}

		Binary Element::serialize() const
		{
			return structure::serialize(*this);
		}

		unsigned Element::index1() const
		{
			rpg2k_assert( hasOwner() );
			return index1_;
		}
		unsigned Element::index2() const
		{
			rpg2k_assert( owner().descriptor().type() == ElementType::Array2D_ );
			return index2_;
		}

		Element const& Element::owner() const
		{
			rpg2k_assert( hasOwner() );
			return *owner_;
		}
		Element& Element::owner()
		{
			rpg2k_assert( hasOwner() );
			return *owner_;
		}

		void Element::substantiate()
		{
			if( descriptor_ && descriptor_->hasDefault() ) switch( descriptor_->type() ) {
				#define PP_enum(TYPE) \
					case ElementType::TYPE##_: \
						if( (*impl_.TYPE##_) == static_cast<TYPE const&>(*descriptor_) ) { \
							exists_ = false; \
							return; \
						} \
						break;
				PP_basicTypes(PP_enum)
				#undef PP_enum

				#define PP_enum(TYPE) case ElementType::TYPE##_:
				PP_rpg2kTypes(PP_enum)
				#undef PP_enum
					break;
				default: rpg2k_assert(false);
			}

			exists_ = true;

			if( hasOwner() ) {
				owner_->substantiate();

				if( owner().descriptor().type() == ElementType::Array2D_ ) {
					( *owner().impl_.Array2D_ )[index1()].substantiate();
				}
			}
		}

		Descriptor const& Element::descriptor() const
		{
			rpg2k_assert( isDefined() );
			return *descriptor_;
		}

		#define PP_castOperator(TYPE) \
			Element::operator TYPE const&() const \
			{ \
				rpg2k_assert( this->isDefined() ); \
				rpg2k_assert( descriptor_->type() == ElementType::TYPE##_ ); \
				rpg2k_assert( impl_.TYPE##_ ); \
				return *impl_.TYPE##_; \
			} \
			Element::operator TYPE&() \
			{ \
				rpg2k_assert( this->isDefined() ); \
				rpg2k_assert( descriptor_->type() == ElementType::TYPE##_ ); \
				rpg2k_assert( impl_.TYPE##_ ); \
				return *impl_.TYPE##_; \
			}
		PP_basicTypes(PP_castOperator)
		PP_rpg2kTypes(PP_castOperator)
		#undef PP_castOperator
	} // namespace structure
} // namespace rpg2k
