#ifndef _INC__RPG2K__STRUCTURE__DESCRIPTOR_HPP
#define _INC__RPG2K__STRUCTURE__DESCRIPTOR_HPP

#include "Define.hpp"

#include <boost/bimap.hpp>
#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_unordered_map.hpp>


namespace rpg2k
{
	class Binary;

	namespace structure
	{
		class Array1D;
		class Array2D;
		class BerEnum;
		class Element;
		class Event;

		class EventState;
		class Music;
		class Sound;

		typedef RPG2kString string;

		class Descriptor;
		typedef boost::ptr_unordered_map<unsigned, Descriptor> ArrayDefineType;
		typedef ArrayDefineType const& ArrayDefine;
		typedef std::auto_ptr<ArrayDefineType> ArrayDefinePointer;

		#define PP_basicTypes(func) \
			func(int) \
			func(bool) \
			func(string) \
			func(double)
		#define PP_rpg2kTypes(func) \
			func(Array1D) \
			func(Array2D) \
			func(Event) \
			func(BerEnum) \
			func(Binary)

		class ElementType : boost::noncopyable
		{
		public:
			static ElementType const& instance();

			enum Enum {
				#define PP_enum(TYPE) TYPE##_,
				PP_basicTypes(PP_enum)
				PP_rpg2kTypes(PP_enum)
				#undef PP_enum
			};
			Enum toEnum(RPG2kString const& name) const;
			RPG2kString const& toString(Enum e) const;

			ElementType();
		private:
			typedef boost::bimap<Enum, RPG2kString> Table;
			Table table_;
		}; // class ElementType
		class Descriptor
		{
		private:
			ElementType::Enum const type_;
			bool const hasDefault_;

			union {
				#define PP_enum(TYPE) TYPE const* TYPE##_;
				PP_basicTypes(PP_enum)
				#undef PP_enum
				boost::ptr_unordered_map<unsigned, Descriptor>* arrayDefine;
			} impl_;

			operator ArrayDefine() const;
		public:
			Descriptor(Descriptor const& src);
			Descriptor(RPG2kString const& type);
			Descriptor(RPG2kString const& type, RPG2kString const& val);
			Descriptor(RPG2kString const& type, ArrayDefinePointer def);

			~Descriptor();

			#define PP_castOperator(type) operator type const&() const;
			PP_basicTypes(PP_castOperator)
			#undef PP_castOperator
			ArrayDefine arrayDefine() const { return static_cast<ArrayDefine>(*this); }

			operator unsigned const&() const
			{
				return reinterpret_cast<unsigned const&>( static_cast<int const&>(*this) );
			}

			RPG2kString const& typeName() const;
			ElementType::Enum type() const { return type_; }

			bool hasDefault() const { return hasDefault_; }
		}; // class Descriptor
	} // namespace structure
} // namespace rpg2k

#endif // _INC__RPG2K__STRUCTURE__DESCRIPTOR_HPP
