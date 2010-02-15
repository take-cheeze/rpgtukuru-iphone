#ifndef _INC__RPG2K__MODEL__ELEMENT_HPP
#define _INC__RPG2K__MODEL__ELEMENT_HPP

#include "Array1D.hpp"
#include "Event.hpp"

namespace rpg2kLib
{

	namespace model
	{
		class BerEnum;
		class Sound;
		class Music;
		class EventState;
	};

	namespace structure
	{

		using namespace rpg2kLib::model;

		class Element;

#define PP_refType(func, end) \
	func(Array1D)end \
	func(Array2D)end \
	func(Music)end \
	func(Sound)end \
	func(EventState)end \
	func(Event)end \
	func(BerEnum)end \
	func(Binary)end

#define PP_basicType(func, end) \
	func(int32_t)end \
	func(bool)end \
	func(string)end \
	func(double)end

#define PP_allType(func, end) \
	PP_refType(func##Ref, end)end PP_basicType(func, end)

		class Descriptor
		{
		protected:
			class InstanceInterface
			{
			private:
				string TYPE_NAME;
			protected:
			public:
				InstanceInterface(string type) : TYPE_NAME(type) {}
				virtual ~InstanceInterface() {}

#define PP_castOperator(type) virtual operator type() const

				PP_basicType(PP_castOperator,;)

#undef PP_castOperator

				virtual operator ArrayDefine() const;

				string getTypeName() const { return TYPE_NAME; }
			}; // Interface

			class ArrayInfo : public InstanceInterface
			{
			private:
				ArrayDefine ARRAY_DEFINE;
			public:
				ArrayInfo(string type, ArrayDefine info);
				virtual ~ArrayInfo();

				virtual operator ArrayDefine() const { return ARRAY_DEFINE; }
			}; // ArrayInfo

			class FactoryInterface
			{
			public:
				virtual InstanceInterface& create(string type, string val) = 0;
			}; // class FactoryInterface

			class Factory
			{
			public:
				template< typename T >
				class InstanceFactory : public FactoryInterface
				{
				public:
					class Value : public InstanceInterface
					{
					private:
						T DATA;
					public:
						Value(const Value& src) : InstanceInterface( src.getTypeName() ), DATA(src.DATA) {}
						Value(string type, T val) : InstanceInterface(type), DATA(val) {}
						virtual ~Value() {}

						virtual operator T() const { return DATA; }
					}; // Value

					T convert(string val);
				public:
					virtual InstanceInterface& create(string type, string val)
					{
						return *new Value( type, convert(val) );
					}
				}; // class InstanceFactory
			private:
				Map< string, FactoryInterface > FACTORY;
			protected:
				Factory();
				Factory(const Factory& i);
			public:
				static Factory& getInstance();

				InstanceInterface& create(string type);
				InstanceInterface& create(string type, string val);
				InstanceInterface& create(string type, ArrayDefine def);

				InstanceInterface& copy(const InstanceInterface& src);
			}; // class Factory
		private:
			InstanceInterface& VALUE;
			bool HAS_DEFAULT;
		public:
			Descriptor(string type);
			Descriptor(string type, string val);
			Descriptor(string type, ArrayDefine def);

			Descriptor(const Descriptor& src);

			virtual ~Descriptor();

			bool hasDefault() { return HAS_DEFAULT; }

			string getTypeName() const { return VALUE.getTypeName(); }
			ArrayDefine getArrayDefine() const { return VALUE; }

#define PP_castOperator(type) operator type() const { return VALUE; }

			PP_basicType(PP_castOperator, )

#undef PP_castOperator

			operator uint() const { return (int)(*this); }
		}; // class Descriptor

#define PP_template(type) template< > type Descriptor::Factory::InstanceFactory< type >::convert(string val)

		PP_basicType(PP_template, ;)

#undef PP_template

		class Element
		{
		protected:
			class InstanceInterface
			{
			private:
				Element& OWNER;
				const Descriptor* DESCRIPTOR;
				Binary BIN_DATA;

				bool EXISTS;
			protected:
				Binary& getBinary() { return BIN_DATA; }
				Element& toElement() { return OWNER; }
			public:
				InstanceInterface(Element& e);
				InstanceInterface(Element& e, Binary& b);

				InstanceInterface(Element& e, const Descriptor& info);
				InstanceInterface(Element& e, const Descriptor& info, Binary& b);
				InstanceInterface(Element& e, const Descriptor& info, Stream& s);

				bool isUndefined() { return DESCRIPTOR == NULL; }

				virtual ~InstanceInterface() {}

				virtual bool exists() const { return EXISTS; }
				virtual void substantiate() { EXISTS = true; }

				const Descriptor& getDescriptor() const;

				virtual const Binary& toBinary() { return BIN_DATA; }

#define PP_castOperator(type) virtual operator type&()
#define PP_castOperatorRef(type) virtual operator type&()

				PP_allType(PP_castOperator,;)

#undef PP_castOperator
#undef PP_castOperatorRef
			};

			class FactoryInterface
			{
			public:
				virtual InstanceInterface& create(Element& e, const Descriptor& info) = 0;
				virtual InstanceInterface& create(Element& e, const Descriptor& info, Binary& b) = 0;
				virtual InstanceInterface& create(Element& e, const Descriptor& info, Stream& s) = 0;
			}; // class FactoryInterface

			class Factory
			{
			protected:
				template< typename T >
				class InstanceFactory : public FactoryInterface
				{
				protected:
					class Instance : public InstanceInterface
					{
					private:
						T DATA;
					protected:
						void init() { getBinary().reset(0); }
					public:
						Instance(Element& e, const Descriptor& info)
							: InstanceInterface(e, info), DATA()
						{
							init();
						}
						Instance(Element& e, const Descriptor& info, Binary& b)
							: InstanceInterface(e, info, b), DATA(b)
						{
							init();
						}
						Instance(Element& e, const Descriptor& info, Stream& s)
							: InstanceInterface(e, info, s), DATA()
						{
							throw "Not supported";
						}

						virtual operator T&()
						{
							if( !exists() ) DATA = static_cast< T >( getDescriptor() );
							return DATA;
						}

						virtual const Binary& toBinary() { getBinary() = DATA; return toBinary(); }
					}; // class Instance
				public:
					virtual InstanceInterface& create(Element& e, const Descriptor& info)
					{
						return *new Instance(e, info);
					}
					virtual InstanceInterface& create(Element& e, const Descriptor& info, Binary& b)
					{
						return *new Instance(e, info, b);
					}
					virtual InstanceInterface& create(Element& e, const Descriptor& info, Stream& s)
					{
						return *new Instance(e, info, s);
					}
				}; // class InstanceFactory

				template< class T >
				class RefInstanceFactory : public FactoryInterface
				{
				protected:
					class RefInstance : public InstanceInterface
					{
					private:
						T DATA;
					protected:
						void init() { getBinary().reset(0); }
					public:
						RefInstance(Element& e, const Descriptor& info)
							: InstanceInterface(e, info), DATA(e, info)
						{
							init();
						}
						RefInstance(Element& e, const Descriptor& info, Binary& b)
							: InstanceInterface(e, info, b), DATA(e, info, b)
						{
							init();
						}
						RefInstance(Element& e, const Descriptor& info, Stream& s)
							: InstanceInterface(e, info, s), DATA(e, info, s)
						{
							init();
						}

						virtual operator T&() { return DATA; }

						virtual const Binary& toBinary() { return DATA.toBinary(); }
					}; // class RefInstance
				public:
					virtual InstanceInterface& create(Element& e, const Descriptor& info)
					{
						return *new RefInstance(e, info);
					}
					virtual InstanceInterface& create(Element& e, const Descriptor& info, Binary& b)
					{
						return *new RefInstance(e, info, b);
					}
					virtual InstanceInterface& create(Element& e, const Descriptor& info, Stream& s)
					{
						return *new RefInstance(e, info, s);
					}
				}; // class RefInstanceFactory
			private:
				Map< string, FactoryInterface > FACTORY;
			protected:
				Factory();
				Factory(const Factory& i);
			public:
				static Factory& getInstance();

				InstanceInterface& create(Element& e, const Descriptor& info);
				InstanceInterface& create(Element& e, const Descriptor& info, Binary& b);
				InstanceInterface& create(Element& e, const Descriptor& info, Stream& s);

				InstanceInterface& create(Element& e);
				InstanceInterface& create(Element& e, Binary& b);

				void dispose(InstanceInterface& i);
			}; // class Factory
		private:
			InstanceInterface& INSTANCE;

			Element* OWNER;
			uint INDEX_1, INDEX_2;
		protected:
			void clear();

			void init() { OWNER = NULL; }
			void init(Element& e) { OWNER = &e; }
		public:
			Element(const Element& e);
			virtual ~Element();

			Element(const Descriptor& info, Binary& b);
			Element(const Descriptor& info, Stream& f);

			Element(const Array1D& owner, uint index);
			Element(const Array1D& owner, uint index , Binary& b);
			Element(const Array2D& owner, uint index1, uint index2);
			Element(const Array2D& owner, uint index1, uint index2, Binary& b);

			bool exists() const { return INSTANCE.exists(); }
			void substantiate();

			bool isUndefined() { return INSTANCE.isUndefined(); }

			const Descriptor& getDescriptor() const { return INSTANCE.getDescriptor(); }

			bool hasOwner() const { return OWNER != NULL; }
			Element& getOwner() const;

			uint getIndex1() const;
			uint getIndex2() const;

#define PP_castOperator(type) \
	operator type&() const { return INSTANCE; } \
	type& get_##type() const { return INSTANCE; } \
	type operator =(type src)
#define PP_castOperatorRef(type) \
	operator type&() const { return INSTANCE; } \
	type& get##type() const { return INSTANCE; } \
	type& operator =(type& src)

			PP_allType(PP_castOperator, ;)

#undef PP_castOperator
#undef PP_castOperatorRef

			operator uint() { return static_cast< int32_t >(*this); }

			uint32_t operator =(uint32_t num) { get_int32_t() = num; return num; }
			uint32_t get_uint() const { return static_cast< int32_t >(*this); }
			 int32_t get_int () const { return static_cast< int32_t >(*this); }

/*
			operator const char*() const { return static_cast< string& >(*this).c_str(); }
			const char* operator =(const char* str)
			{
				static_cast< string& >(*this) = str; return str;
			}
 */

			const Binary& toBinary() { return INSTANCE.toBinary(); }
		}; // class Element

#define PP_operator(retType, op, type) \
	inline retType operator op(Element& e, type in) { return static_cast< type >(e) op in; } \
	inline retType operator op(type in, Element& e) { return in op static_cast< type >(e); }

		PP_operator(bool, ==, string&);
		PP_operator(bool, !=, string&);

		PP_operator(string, +, string);

#undef PP_mathOperator

#undef PP_operator

	}; // namespace structure

}; // namespace rpg2kLib

#endif
