#include "Descriptor.hpp"
#include "Structure.hpp"


namespace rpg2k
{
	namespace structure
	{
		class Descriptor::ArrayInfo : public InstanceInterface
		{
		private:
			ArrayDefinePointer arrayDefinePointer_;
		public:
			ArrayInfo(RPG2kString const& type, ArrayDefinePointer info)
			: InstanceInterface(type, true), arrayDefinePointer_(info)
			{
			}
			virtual ~ArrayInfo()
			{
			}

			virtual operator ArrayDefinePointer() const { return arrayDefinePointer_; }
		}; // ArrayInfo

		class Descriptor::Factory
		{
		public:
			class FactoryInterface
			{
			public:
				virtual InstancePointer create(RPG2kString const& type, RPG2kString const& val) = 0;
			}; // class FactoryInterface

			template< typename T >
			class FactoryInstance : public FactoryInterface
			{
			public:
				class Value : public InstanceInterface
				{
				private:
					T const data_;
				public:
					Value(Value const& src) : InstanceInterface( src.getTypeName(), true ), data_(src.data_) {}
					Value(RPG2kString const& type, T val) : InstanceInterface(type, true), data_(val) {}
					virtual ~Value() {}

					virtual operator T() const { return data_; }
				}; // class Value

				T convert(RPG2kString const& val)
				{
					std::istringstream ss(val);
					T ret;

					ss.exceptions(std::ios_base::failbit | std::ios_base::badbit);
					ss >> std::boolalpha >> ret;

					return ret;
				}
			public:
				virtual InstancePointer create(RPG2kString const& type, RPG2kString const& val)
				{
					return InstancePointer( new Value( type, convert(val) ) );
				}
			}; // class FactoryInstance
		private:
			Map< RPG2kString, FactoryInterface > factory_;
		protected:
			Factory()
			{
				#define PP_enum(type) factory_.addPointer( #type, boost::shared_ptr< FactoryInterface >( new FactoryInstance< type >() ) );
				PP_basicType(PP_enum)
				#undef PP_enum
			}
			Factory(Factory const& i);
		public:
			static Factory& instance()
			{
				static Factory theFactory;
				return theFactory;
			}

			InstancePointer create(RPG2kString const& type)
			{
				return InstancePointer( new InstanceInterface(type) );
			}
			InstancePointer create(RPG2kString const& type, RPG2kString const& val)
			{
				return factory_[type].create(type, val);
			}
			InstancePointer create(RPG2kString const& type, ArrayDefinePointer def)
			{
				return InstancePointer( new ArrayInfo(type, def) );
			}

			InstancePointer copy(Descriptor::InstanceInterface const& src)
			{
				if( !src.hasDefault() ) return InstancePointer( new InstanceInterface( src.getTypeName() ) );
				#define PP_enum(type) \
					else if( src.getTypeName() == #type ) \
						return InstancePointer( \
							new FactoryInstance< type >::Value( (FactoryInstance< type >::Value&)src ) \
						);
				PP_basicType(PP_enum)
				#undef PP_enum
				else return InstancePointer( new ArrayInfo( src.getTypeName(), src ) );
			}
		}; // class Factory

		template< >
		RPG2kString Descriptor::Factory::FactoryInstance< RPG2kString >::convert(RPG2kString const& val)
		{
			if( (val.length() >= 2) && (*val.begin() == '\"') && (*val.rbegin() == '\"') ) {
				return RPG2kString( ++val.begin(), --val.end() );
			} else return val;
		}

		Descriptor::Descriptor(RPG2kString const& type)
		: value_( Factory::instance().create(type) )
		{
		}
		Descriptor::Descriptor(RPG2kString const& type, RPG2kString const& val)
		: value_( Factory::instance().create(type, val) )
		{
		}
		Descriptor::Descriptor(RPG2kString const& type, ArrayDefinePointer def)
		: value_( Factory::instance().create(type, def) )
		{
		}

		Descriptor::Descriptor(Descriptor const& src)
		: value_( Factory::instance().copy(*src.value_) )
		{
		}

		Descriptor::~Descriptor()
		{
		}


		#define PP_castOperator(type) \
			Descriptor::InstanceInterface::operator type()  const\
			{ \
				throw std::runtime_error( "Not supported at type: " + getTypeName() ); \
			}
		PP_basicType(PP_castOperator)
		PP_castOperator(ArrayDefinePointer)
		#undef PP_castOperator
	} // namespace structure
} // namespace rpg2k
