#ifndef _INC__RPG2K__STRUCTURE__DESCRIPTOR_HPP
#define _INC__RPG2K__STRUCTURE__DESCRIPTOR_HPP

#include "Define.hpp"
#include "Map.hpp"


namespace rpg2k
{
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

		#define PP_refType(func) \
			func(Array1D) func(Array2D) \
			func(Music) func(Sound) \
			func(EventState) \
			func(Event) \
			func(BerEnum) \
			func(Binary)
		#define PP_basicType(func) \
			func(int) \
			func(bool) \
			func(string) \
			func(double)
		#define PP_allType(func) \
			PP_refType(func##Ref) PP_basicType(func)

		class Descriptor
		{
		private:
			RPG2kString const typeName_;
			bool const hasDefault_;
		protected:
			class ArrayInfo;
			class Factory;

			Descriptor(RPG2kString const& type, bool hasDef = false) : typeName_(type), hasDefault_(hasDef) {}

			virtual operator ArrayDefine() const;
		public:
			virtual ~Descriptor() {}

			#define PP_castOperator(type) virtual operator type() const;
			PP_basicType(PP_castOperator)
			#undef PP_castOperator
			ArrayDefine getArrayDefine() const { return static_cast<ArrayDefine>(*this); }

			operator uint() const { return static_cast<int>(*this); }

			RPG2kString const& getTypeName() const { return typeName_; }

			bool hasDefault() const { return hasDefault_; }

			static std::auto_ptr< Descriptor > create(RPG2kString const& type);
			static std::auto_ptr< Descriptor > create(RPG2kString const& type, RPG2kString const& val);
			static std::auto_ptr< Descriptor > create(RPG2kString const& type, ArrayDefinePointer def);
			static std::auto_ptr< Descriptor > copy(Descriptor const& src);
		}; // class Descriptor
	} // namespace structure
} // namespace rpg2k

#endif // _INC__RPG2K__STRUCTURE__DESCRIPTOR_HPP
