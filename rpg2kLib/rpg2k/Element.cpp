#include "Array1D.hpp"
#include "Array2D.hpp"
#include "Debug.hpp"
#include "Element.hpp"
#include "Event.hpp"
#include "SpecialArray1D.hpp"


namespace rpg2k
{
	namespace structure
	{
		// check serialize is successed
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
				virtual InstancePointer create(Element& e, Descriptor const& info) = 0;
				virtual InstancePointer create(Element& e, Descriptor const& info, Binary const& b) = 0;
				virtual InstancePointer create(Element& e, Descriptor const& info, StreamReader& s) = 0;
			}; // class Element::FactoryInterface

			template< typename T >
			class FactoryInstance : public FactoryInterface
			{
			protected:
				class Instance : public InstanceInterface
				{
				private:
					T data_;
				protected:
					void init() { getBinary().resize(0); }
				public:
					Instance(Element& e, Descriptor const& info)
					: InstanceInterface(e, info), data_()
					{
						if( isDefined() && info.hasDefault() ) data_ = static_cast< T >(info);
						init();
					}
					Instance(Element& e, Descriptor const& info, Binary const& b)
					: InstanceInterface(e, info, b), data_(b)
					{
						init();
					}
					Instance(Element& e, Descriptor const& info, StreamReader& s)
					: InstanceInterface(e, info, s), data_()
					{
						rpg2k_assert(false);
					}

					virtual operator T&()
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
				virtual InstancePointer create(Element& e, Descriptor const& info)
				{
					return InstancePointer( new Instance(e, info) );
				}
				virtual InstancePointer create(Element& e, Descriptor const& info, Binary const& b)
				{
					return InstancePointer( new Instance(e, info, b) );
				}
				virtual InstancePointer create(Element& e, Descriptor const& info, StreamReader& s)
				{
					return InstancePointer( new Instance(e, info, s) );
				}
			}; // class FactoryInstance

			template< class T >
			class RefFactoryInstance : public FactoryInterface
			{
			protected:
				class RefInstance : public InstanceInterface
				{
				private:
					T data_;
				protected:
					void init() { getBinary().resize(0); }
				public:
					RefInstance(Element& e, Descriptor const& info)
					: InstanceInterface(e, info), data_(e, info)
					{
						init();
					}
					RefInstance(Element& e, Descriptor const& info, Binary const& b)
					: InstanceInterface(e, info, b), data_(e, info, b)
					{
						init();
					}
					RefInstance(Element& e, Descriptor const& info, StreamReader& s)
					: InstanceInterface(e, info), data_(e, info, s)
					{
						init();
					}

					virtual operator T&() { return data_; }

					virtual uint serializedSize() const { return data_.serializedSize(); }
					virtual void serialize(StreamWriter& s) const { data_.serialize(s); }
				}; // class RefInstance
			public:
				virtual InstancePointer create(Element& e, Descriptor const& info)
				{
					return InstancePointer( new RefInstance(e, info) );
				}
				virtual InstancePointer create(Element& e, Descriptor const& info, Binary const& b)
				{
					return InstancePointer( new RefInstance(e, info, b) );
				}
				virtual InstancePointer create(Element& e, Descriptor const& info, StreamReader& s)
				{
					return InstancePointer( new RefInstance(e, info, s) );
				}
			}; // class RefFactoryInstance
		private:
			Map< RPG2kString, FactoryInterface > factory_;
		protected:
			Factory(Factory const& f);
			Factory()
			{
				#define PP_enum(type) factory_.addPointer( #type, boost::shared_ptr< FactoryInterface >( new FactoryInstance< type >() ) );
				#define PP_enumRef(type) factory_.addPointer( #type, boost::shared_ptr< FactoryInterface >( new RefFactoryInstance< type >() ) );
				PP_allType(PP_enum)
				#undef PP_enum
				#undef PP_enumRef
			}
		public:
			static Factory& instance()
			{
				static Factory theFactory;
				return theFactory;
			}

			InstancePointer create(Element& e, Descriptor const& info)
			{
				return factory_[info.getTypeName()].create(e, info);
			}
			InstancePointer create(Element& e, Descriptor const& info, Binary const& b)
			{
				return factory_[info.getTypeName()].create(e, info, b);
			}
			InstancePointer create(Element& e, Descriptor const& info, StreamReader& s)
			{
				return factory_[info.getTypeName()].create(e, info, s);
			}
			InstancePointer create(Element& e)
			{
				return InstancePointer( new InstanceInterface(e) );
			}
			InstancePointer create(Element& e, Binary const& b)
			{
				return InstancePointer( new InstanceInterface(e, b) );
			}
		}; // class Element::Factory

		Element::Element(Element const& e)
		: owner_(e.owner_)
		, index1_(e.index1_), index2_(e.index2_)
		, instance_( Factory::instance().create( *this, e.getDescriptor(), e.serialize() ) )
		{
		}

		Element::Element(Descriptor const& info)
		: owner_(NULL)
		, index1_(INVALID_INDEX), index2_(INVALID_INDEX)
		, instance_( Factory::instance().create(*this, info) )
		{
		}
		Element::Element(Descriptor const& info, Binary const& b)
		: owner_(NULL)
		, index1_(INVALID_INDEX), index2_(INVALID_INDEX)
		, instance_( Factory::instance().create(*this, info, b) )
		{
			#if RPG2K_CHECK_AT_CONSTRUCTOR
				rpg2k_assert( checkSerialize(instance_, b) );
			#endif
		}
		Element::Element(Descriptor const& info, StreamReader& s)
		: owner_(NULL)
		, index1_(INVALID_INDEX), index2_(INVALID_INDEX)
		, instance_( Factory::instance().create(*this, info, s) )
		{
		}

		Element::Element(Array1D const& owner, uint index)
		: owner_( &owner.toElement() )
		, index1_(index), index2_(INVALID_INDEX)
		, instance_( owner.getArrayDefine().exists(index)
				? Factory::instance().create(*this, owner.getArrayDefine()[index])
				: Factory::instance().create(*this)
			)
		{
		}
		Element::Element(Array1D const& owner, uint index , Binary const& b)
		: owner_( &owner.toElement() )
		, index1_(index), index2_(INVALID_INDEX)
		, instance_( owner.getArrayDefine().exists(index)
				? Factory::instance().create(*this, owner.getArrayDefine()[index], b)
				: Factory::instance().create(*this, b)
			)
		{
			#if RPG2K_CHECK_AT_CONSTRUCTOR
				rpg2k_assert( instance_.checkSerialize(b) );
			#endif
		}
		Element::Element(Array2D const& owner, uint index1, uint index2)
		: owner_( &owner.toElement() )
		, index1_(index1), index2_(index2)
		, instance_( owner.getArrayDefine().exists(index2)
				? Factory::instance().create(*this, owner.getArrayDefine()[index2])
				: Factory::instance().create(*this)
			)
		{
		}
		Element::Element(Array2D const& owner, uint index1, uint index2, Binary const& b)
		: owner_( &owner.toElement() )
		, index1_(index1), index2_(index2)
		, instance_( owner.getArrayDefine().exists(index2)
				? Factory::instance().create(*this, owner.getArrayDefine()[index2], b)
				: Factory::instance().create(*this, b)
			)
		{
			#if RPG2K_CHECK_AT_CONSTRUCTOR
				rpg2k_assert( instance_.checkSerialize(b) );
			#endif
		}

		Element::~Element()
		{
			#if RPG2K_ANALYZE_AT_DECONSTRUCTOR
				if( exists() ) {
					#if RPG2K_ONLY_ANALYZE_NON_DEFINED_ELEMENT
						if( isDefined() ) debug::Tracer::printTrace(*this, true, cout);
					#else
						debug::Tracer::printTrace(*this, true, cout);
					#endif
				}
			#endif
		}

		Element& Element::operator =(Element const& src)
		{
			#define PP_enum(type) \
				if( src.getDescriptor().getTypeName() == #type ) \
					(*this) = static_cast< type& >(src); \
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
			ArrayDefine def = getOwner().getDescriptor().getArrayDefine();
			def.exists(index1_);

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

		void Element::clear()
		{
		}

		void Element::substantiate()
		{
			instance_->substantiate();

			if( hasOwner() ) {
				owner_->substantiate();

				// clog << getOwner().getDescriptor().getTypeName() << endl;

				if( getOwner().getDescriptor().getTypeName() == "Array2D" ) {
					getOwner().getArray2D()[getIndex1()].substantiate();
				}
			}
		}

		Element::InstanceInterface::InstanceInterface(Element& e)
		: owner_(e), descriptor_(NULL), exists_(true)
		{
		}
		Element::InstanceInterface::InstanceInterface(Element& e, Binary const& b)
		: owner_(e), descriptor_(NULL), binData_(b), exists_(true)
		{
		}
		Element::InstanceInterface::InstanceInterface(Element& e, Descriptor const& info)
		: owner_(e), descriptor_(&info), exists_(false)
		{
		}
		Element::InstanceInterface::InstanceInterface(Element& e, Descriptor const& info, Binary const& b)
		: owner_(e), descriptor_(&info), binData_(b), exists_(true)
		{
		}
		Element::InstanceInterface::InstanceInterface(Element& e, Descriptor const& info, StreamReader& s)
		: owner_(e), descriptor_(&info), exists_(true)
		{
			rpg2k_assert(false);
		}

		Descriptor const& Element::InstanceInterface::getDescriptor() const
		{
			rpg2k_assert( isDefined() );
			return *descriptor_;
		}

		#define PP_castOperator(type) \
			Element::InstanceInterface::operator type&() \
			{ \
				throw std::runtime_error( "Not supported at type: " + getDescriptor().getTypeName() ); \
			}
		#define PP_castOperatorRef(type) PP_castOperator(type)
		PP_allType(PP_castOperator)
		#undef PP_castOperator
		#undef PP_castOperatorRef
	} // namespace structure
} // namespace rpg2k
