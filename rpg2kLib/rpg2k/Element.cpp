#include "Array1D.hpp"
#include "Array2D.hpp"
#include "Debug.hpp"
#include "Element.hpp"
#include "Event.hpp"
#include "SpecialArray1D.hpp"


#if RPG2K_ANALYZE_AT_DECONSTRUCTOR
	#if RPG2K_ONLY_ANALYZE_NON_DEFINED_ELEMENT
		#define ANALYZE_SELF() \
			if( exists() ) if( isDefined() ) debug::Tracer::printTrace(*this, true)
	#else
		#define ANALYZE_SELF() \
			if( exists() ) debug::Tracer::printTrace(*this, true)
	#endif
#else
	#define ANALYZE_SELF
#endif

namespace rpg2k
{
	namespace structure
	{
		template< class T >
		bool checkSerialize(T const& result, Binary const& src)
		{
			Binary serialized = structure::serialize(result);
			if(src != serialized) {
				debug::Tracer::printBinary(src, cerr);
				cerr << endl;
				debug::Tracer::printBinary(serialized, cerr);
				cerr << endl;

				return false;
			}
			return true;
		}

		BerEnum::BerEnum(Element& e, Descriptor const& info, StreamReader& s)
		{
			init(s);
		}
		BerEnum::BerEnum(Element& e, Descriptor const& info, Binary const& b)
		{
			StreamReader s(b);
			init(s);
			rpg2k_assert( s.eof() );
		}

		void BerEnum::init(StreamReader& s)
		{
			uint length = s.getBER();

			resize(length+1);
			for(uint i = 0; i <= length; i++) (*this)[i] = s.getBER();
		}

		uint BerEnum::serializedSize() const
		{
			uint ret = getBERSize( size() - 1 );
			for(uint i = 0; i < size(); i++) ret += getBERSize( (*this)[i] );

			return ret;
		}
		void BerEnum::serialize(StreamWriter& s) const
		{
			Binary b = ( std::vector< uint >& ) *this;
			s.setBER( b.size() - 1 );
			s.write(b);
		}
/*
		CharSetDir EventState::charSetDir() const
		{
			return (CharSetDir) (*this)[30 + talkDir()].get<int>();
		}
  */

		class Element::Factory
		{
		protected:
			class FactoryInterface
			{
			public:
				virtual std::auto_ptr<Element> create(Descriptor const& info) = 0;
				virtual std::auto_ptr<Element> create(Descriptor const& info, Binary const& b) = 0;
				virtual std::auto_ptr<Element> create(Descriptor const& info, StreamReader& s) = 0;
			}; // class Element::FactoryInterface

			template< typename T >
			class FactoryInstance : public FactoryInterface
			{
			protected:
				class Instance : public Element
				{
				private:
					T data_;
				protected:
					void init() { getData().resize(0); }
				public:
					Instance(Descriptor const& info)
					: Element(info), data_()
					{
						if( isDefined() && info.hasDefault() ) data_ = static_cast< T >(info);
						init();
					}
					Instance(Descriptor const& info, Binary const& b)
					: Element(info, b), data_(b)
					{
						init();
					}
					Instance(Descriptor const& info, StreamReader& s)
					: Element(info, s), data_()
					{
						rpg2k_assert(false);
					}
					virtual ~Instance()
					{
						ANALYZE_SELF();
					}

					virtual operator T const&() const
					{
						rpg2k_assert( exists() || isDefined() );
						return data_;
					}
					virtual uint serializedSize() const
					{
						Binary b;
						b = data_;
						return b.size();
					}
					virtual void serialize(StreamWriter& s) const
					{
						Binary b;
						b = data_;
						s.write(b);
					}
				}; // class Instance
			public:
				virtual std::auto_ptr<Element> create(Descriptor const& info)
				{
					return std::auto_ptr<Element>( new Instance(info) );
				}
				virtual std::auto_ptr<Element> create(Descriptor const& info, Binary const& b)
				{
					return std::auto_ptr<Element>( new Instance(info, b) );
				}
				virtual std::auto_ptr<Element> create(Descriptor const& info, StreamReader& s)
				{
					return std::auto_ptr<Element>( new Instance(info, s) );
				}
			}; // class FactoryInstance

			template< class T >
			class RefFactoryInstance : public FactoryInterface
			{
			protected:
				class RefInstance : public Element
				{
				private:
					T data_;
				protected:
					void init() { getData().resize(0); }
				public:
					RefInstance(Descriptor const& info)
					: Element(info), data_(*this, info)
					{
						init();
					}
					RefInstance(Descriptor const& info, Binary const& b)
					: Element(info, b), data_(*this, info, b)
					{
						init();
					}
					RefInstance(Descriptor const& info, StreamReader& s)
					: Element(info), data_(*this, info, s)
					{
						init();
					}
					virtual ~RefInstance()
					{
						ANALYZE_SELF();
					}

					virtual operator T const&() const { return data_; }

					virtual uint serializedSize() const { return data_.serializedSize(); }
					virtual void serialize(StreamWriter& s) const { data_.serialize(s); }
				}; // class RefInstance
			public:
				virtual std::auto_ptr<Element> create(Descriptor const& info)
				{
					return std::auto_ptr<Element>( new RefInstance(info) );
				}
				virtual std::auto_ptr<Element> create(Descriptor const& info, Binary const& b)
				{
					return std::auto_ptr<Element>( new RefInstance(info, b) );
				}
				virtual std::auto_ptr<Element> create(Descriptor const& info, StreamReader& s)
				{
					return std::auto_ptr<Element>( new RefInstance(info, s) );
				}
			}; // class RefFactoryInstance
		private:
			Map< RPG2kString, FactoryInterface > factory_;
		protected:
			Factory(Factory const& f);
			Factory()
			{
				#define PP_enum(type) factory_.addPointer( #type, std::auto_ptr<FactoryInterface>( new FactoryInstance< type >() ) );
				#define PP_enumRef(type) factory_.addPointer( #type, std::auto_ptr<FactoryInterface>( new RefFactoryInstance< type >() ) );
				PP_allType(PP_enum)
				#undef PP_enum
				#undef PP_enumRef

				#define PP_enum(type) factory_.addPointer( #type, std::auto_ptr<FactoryInterface>( new RefFactoryInstance<Array1D>() ) )
				PP_enum(Music);
				PP_enum(Sound);
				PP_enum(EventState);
				#undef PP_enum
			}
		public:
			static Factory& instance()
			{
				static Factory theFactory;
				return theFactory;
			}

			std::auto_ptr<Element> create(Descriptor const& info)
			{
				return factory_[info.getTypeName()].create(info);
			}
			std::auto_ptr<Element> create(Descriptor const& info, Binary const& b)
			{
				return factory_[info.getTypeName()].create(info, b);
			}
			std::auto_ptr<Element> create(Descriptor const& info, StreamReader& s)
			{
				return factory_[info.getTypeName()].create(info, s);
			}
			std::auto_ptr<Element> create()
			{
				return std::auto_ptr<Element>( new Element() );
			}
			std::auto_ptr<Element> create(Binary const& b)
			{
				return std::auto_ptr<Element>( new Element(b) );
			}
		}; // class Element::Factory

		std::auto_ptr<Element> Element::copy(Element const& e)
		{
			std::auto_ptr<Element> ret =  Factory::instance().create( e.getDescriptor(), e.serialize() ) ;
			ret->owner_ = e.owner_;
			ret->index1_ = e.index1_; ret->index2_ = e.index2_;

			return ret;
		}

		std::auto_ptr<Element> Element::create(Descriptor const& info)
		{
			std::auto_ptr<Element> ret = Factory::instance().create(info);

			return ret;
		}
		std::auto_ptr<Element> Element::create(Descriptor const& info, Binary const& b)
		{
			std::auto_ptr<Element> ret = Factory::instance().create(info, b);
			#if RPG2K_CHECK_AT_CONSTRUCTOR
				rpg2k_assert( checkSerialize(instance_, b) );
			#endif

			return ret;
		}
		std::auto_ptr<Element> Element::create(Descriptor const& info, StreamReader& s)
		{
			std::auto_ptr<Element> ret = Factory::instance().create(info, s);

			return ret;
		}

		std::auto_ptr<Element> Element::create(Array1D const& owner, uint index)
		{
			std::auto_ptr<Element> ret = owner.getArrayDefine().exists(index)
				? Factory::instance().create(owner.getArrayDefine()[index])
				: Factory::instance().create()
				;
			ret->owner_ =  &owner.toElement() ;
			ret->index1_ = index;

			return ret;
		}
		std::auto_ptr<Element> Element::create(Array1D const& owner, uint index, Binary const& b)
		{
			std::auto_ptr<Element> ret = owner.getArrayDefine().exists(index)
				? Factory::instance().create(owner.getArrayDefine()[index], b)
				: Factory::instance().create(b)
				;
			ret->owner_ =  &owner.toElement() ;
			ret->index1_ = index;
			#if RPG2K_CHECK_AT_CONSTRUCTOR
				rpg2k_assert( instance_.checkSerialize(b) );
			#endif

			return ret;
		}
		std::auto_ptr<Element> Element::create(Array2D const& owner, uint index1, uint index2)
		{
			std::auto_ptr<Element> ret = owner.getArrayDefine().exists(index2)
				? Factory::instance().create(owner.getArrayDefine()[index2])
				: Factory::instance().create()
				;
			ret->owner_ =  &owner.toElement() ;
			ret->index1_ = index1; ret->index2_ = index2;

			return ret;
		}
		std::auto_ptr<Element> Element::create(Array2D const& owner, uint index1, uint index2, Binary const& b)
		{
			std::auto_ptr<Element> ret = owner.getArrayDefine().exists(index2)
				? Factory::instance().create(owner.getArrayDefine()[index2], b)
				: Factory::instance().create(b)
				;
			ret->owner_ =  &owner.toElement() ;
			ret->index1_ = index1; ret->index2_ = index2;
			#if RPG2K_CHECK_AT_CONSTRUCTOR
				rpg2k_assert( instance_.checkSerialize(b) );
			#endif

			return ret;
		}

		Element::~Element()
		{
			if( !isDefined() ) ANALYZE_SELF();
		}

		Element& Element::operator =(Element const& src)
		{
			#define PP_enum(type) \
				if( src.getDescriptor().getTypeName() == #type ) \
					(*this) = static_cast< type& >( const_cast<Element&>(src) ); \
				else
			#define PP_enumRef(type) PP_enum(type)
			PP_allType(PP_enum)
			#undef PP_enum
			#undef PP_enumRef
			/* else */ rpg2k_assert(false);

			return *this;
		}

		uint Element::getIndex1() const
		{
			rpg2k_assert( hasOwner() );
			return index1_;
		}
		uint Element::getIndex2() const
		{
			rpg2k_assert( getOwner().getDescriptor().getTypeName() == "Array2D" );
			return index2_;
		}

		Element& Element::getOwner() const
		{
			rpg2k_assert( hasOwner() );
			return *owner_;
		}

		void Element::substantiate()
		{
			exists_ = true;;

			if( hasOwner() ) {
				owner_->substantiate();

				// clog << getOwner().getDescriptor().getTypeName() << endl;

				if( getOwner().getDescriptor().getTypeName() == "Array2D" ) {
					getOwner().getArray2D()[getIndex1()].substantiate();
				}
			}
		}

		Element::Element()
		: descriptor_(NULL), exists_(false), owner_(NULL)
		{
		}
		Element::Element(Binary const& b)
		: descriptor_(NULL), binData_(b), exists_(true), owner_(NULL)
		{
		}
		Element::Element(Descriptor const& info)
		: descriptor_(&info), exists_(false), owner_(NULL)
		{
		}
		Element::Element(Descriptor const& info, Binary const& b)
		: descriptor_(&info), binData_(b), exists_(true), owner_(NULL)
		{
		}
		Element::Element(Descriptor const& info, StreamReader& s)
		: descriptor_(&info), exists_(true), owner_(NULL)
		{
			rpg2k_assert(false);
		}

		Descriptor const& Element::getDescriptor() const
		{
			rpg2k_assert( isDefined() );
			return *descriptor_;
		}

		#define PP_castOperator(type) \
			Element::operator type const&() const \
			{ \
				throw std::runtime_error( "Not supported at type: " + getDescriptor().getTypeName() ); \
			}
		#define PP_castOperatorRef(type) PP_castOperator(type)
		PP_allType(PP_castOperator)
		#undef PP_castOperator
		#undef PP_castOperatorRef
	} // namespace structure
} // namespace rpg2k
