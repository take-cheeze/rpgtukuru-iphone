#include "Descriptor.hpp"
#include "Structure.hpp"

#include <sstream>


namespace rpg2k
{
	namespace structure
	{
		ElementType::ElementType()
		{
			#define PP_insert(TYPE) table_.insert( Table::value_type( TYPE##_, #TYPE ) );
			PP_basicTypes(PP_insert)
			PP_rpg2kTypes(PP_insert)
			#undef PP_insert
		}
		ElementType::Enum ElementType::toEnum(RPG2kString const& name) const
		{
			Table::right_map::const_iterator it = table_.right.find(name);
			rpg2k_assert( it != table_.right.end() );
			return it->second;
		}
		RPG2kString const& ElementType::toString(ElementType::Enum const e) const
		{
			Table::left_map::const_iterator it = table_.left.find(e);
			rpg2k_assert( it != table_.left.end() );
			return it->second;
		}
		ElementType const& ElementType::instance()
		{
			static ElementType theElementType;
			return theElementType;
		}

		Descriptor::Descriptor(Descriptor const& src)
		: type_(src.type_), hasDefault_(src.hasDefault_)
		{
			if(hasDefault_) switch(type_) {
				#define PP_enum(TYPE) \
					case ElementType::TYPE##_: \
						impl_.TYPE##_ = new TYPE( *src.impl_.TYPE##_ ); \
						return;
				PP_basicTypes(PP_enum)
				#undef PP_enum
				case ElementType::Array1D_:
				case ElementType::Array2D_:
					impl_.arrayDefine =
						new boost::ptr_unordered_map<unsigned, Descriptor>(*src.impl_.arrayDefine);
					break;
				default: rpg2k_assert(false); break;
			}
		}
		Descriptor::Descriptor(RPG2kString const& type)
		: type_( ElementType::instance().toEnum(type) ), hasDefault_(false)
		{
		}
		Descriptor::Descriptor(RPG2kString const& type, RPG2kString const& val)
		: type_( ElementType::instance().toEnum(type) ), hasDefault_(true)
		{
			switch(type_) {
				case ElementType::string_:
					if(
						( val.size() > 2 ) &&
						( *val.begin() == '\"' ) && ( *val.rbegin() == '\"' )
					) impl_.string_ = new RPG2kString( ++val.begin(), --val.end() );
					else impl_.string_ = new RPG2kString(val);
					break;
				#define PP_enum(TYPE) \
					case ElementType::TYPE##_: { \
						TYPE* newed = new TYPE(); \
						std::istringstream iss(val); \
						iss >> std::boolalpha >> (*newed); \
						impl_.TYPE##_ = newed; \
					} break
				PP_enum(double);
				PP_enum(int);
				PP_enum(bool);
				#undef PP_enum
				default: rpg2k_assert(false); break;
			}
		}
		Descriptor::Descriptor(RPG2kString const& type, ArrayDefinePointer def)
		: type_( ElementType::instance().toEnum(type) ), hasDefault_(true)
		{
			rpg2k_assert( (type_ == ElementType::Array1D_) || (type_ == ElementType::Array2D_) );
			impl_.arrayDefine = def.release();
		}

		Descriptor::~Descriptor()
		{
			if(hasDefault_) switch(type_) {
				#define PP_enum(TYPE) case ElementType::TYPE##_: delete impl_.TYPE##_; return;
				PP_basicTypes(PP_enum)
				#undef PP_enum
				case ElementType::Array1D_:
				case ElementType::Array2D_:
					delete impl_.arrayDefine;
					return;
				default: rpg2k_assert(false); break;
			}
		}

		#define PP_castOperator(TYPE) \
			Descriptor::operator TYPE const&() const \
			{ \
				rpg2k_assert(type_ == ElementType::TYPE##_); \
				return *impl_.TYPE##_; \
			}
		PP_basicTypes(PP_castOperator)
		#undef PP_castOperator

		Descriptor::operator ArrayDefine() const
		{
			rpg2k_assert( (type_ == ElementType::Array1D_) || (type_ == ElementType::Array2D_) );
			return *impl_.arrayDefine;
		}

		RPG2kString const& Descriptor::typeName() const
		{
			return ElementType::instance().toString(type_);
		}
	} // namespace structure
} // namespace rpg2k
