#include "Descriptor.hpp"
#include "Structure.hpp"


namespace rpg2k
{
	namespace structure
	{
		class Descriptor::ArrayInfo : public Descriptor
		{
		private:
			ArrayDefinePointer arrayDefinePointer_;

			virtual operator ArrayDefine() const { return *arrayDefinePointer_; }
		public:
			ArrayInfo(RPG2kString const& type, ArrayDefinePointer info)
			: Descriptor(type, true), arrayDefinePointer_(info)
			{
			}
		}; // class Descriptor::ArrayInfo

		class Descriptor::Factory
		{
		public:
			class FactoryInterface
			{
			public:
				virtual std::auto_ptr<Descriptor> create(RPG2kString const& type, RPG2kString const& val) = 0;
			}; // class FactoryInterface

			template< typename T >
			class FactoryInstance : public FactoryInterface
			{
			public:
				class Value : public Descriptor
				{
				private:
					T const data_;

					virtual operator T() const { return data_; }
				public:
					Value(Value const& src) : Descriptor( src.getTypeName(), true ), data_(src.data_) {}
					Value(RPG2kString const& type, T val) : Descriptor(type, true), data_(val) {}
					virtual ~Value() {}
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
				virtual std::auto_ptr<Descriptor> create(RPG2kString const& type, RPG2kString const& val)
				{
					return std::auto_ptr<Descriptor>( new Value( type, convert(val) ) );
				}
			}; // class FactoryInstance
		private:
			Map< RPG2kString, FactoryInterface > factory_;
		protected:
			Factory()
			{
				#define PP_enum(type) factory_.addPointer( #type, std::auto_ptr<FactoryInterface>( new FactoryInstance< type >() ) );
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

			std::auto_ptr<Descriptor> create(RPG2kString const& type, RPG2kString const& val)
			{
				return factory_[type].create(type, val);
			}

			std::auto_ptr<Descriptor> copy(Descriptor const& src)
			{
				if( !src.hasDefault() ) return std::auto_ptr<Descriptor>( new Descriptor( src.getTypeName() ) );
				#define PP_enum(type) \
					else if( src.getTypeName() == #type ) \
						return std::auto_ptr<Descriptor>( \
							new FactoryInstance< type >::Value( dynamic_cast<FactoryInstance< type >::Value const&>(src) ) \
						);
				PP_basicType(PP_enum)
				#undef PP_enum
				else return std::auto_ptr<Descriptor>( new ArrayInfo( src.getTypeName(),
					ArrayDefinePointer( new Map< uint, Descriptor >( static_cast< ArrayDefine >(src) ) ) ) );
			}
		}; // class Descriptor::Factory

		template< >
		RPG2kString Descriptor::Factory::FactoryInstance< RPG2kString >::convert(RPG2kString const& val)
		{
			if( (val.length() >= 2) && (*val.begin() == '\"') && (*val.rbegin() == '\"') ) {
				return RPG2kString( ++val.begin(), --val.end() );
			} else return val;
		}

		std::auto_ptr< Descriptor > Descriptor::create(RPG2kString const& type)
		{
			return std::auto_ptr<Descriptor>( new Descriptor(type) );
		}
		std::auto_ptr< Descriptor > Descriptor::create(RPG2kString const& type, RPG2kString const& val)
		{
			return Factory::instance().create(type, val);
		}
		std::auto_ptr< Descriptor > Descriptor::create(RPG2kString const& type, ArrayDefinePointer def)
		{
			return std::auto_ptr<Descriptor>( new ArrayInfo(type, def) );
		}

		std::auto_ptr< Descriptor > Descriptor::copy(Descriptor const& src)
		{
			return Factory::instance().copy(src);
		}

		#define PP_castOperator(type) \
			Descriptor::operator type()  const\
			{ \
				throw std::runtime_error( "Not supported at type: " + getTypeName() ); \
			}
		PP_basicType(PP_castOperator)
		PP_castOperator(ArrayDefine)
		#undef PP_castOperator
	} // namespace structure
} // namespace rpg2k
