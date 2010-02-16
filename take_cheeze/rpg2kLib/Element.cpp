#include "Debug.hpp"
#include "Encode.hpp"
#include "Element.hpp"
#include "Model.hpp"

using namespace rpg2kLib::debug;
using namespace rpg2kLib::encode;
using namespace rpg2kLib::model;


Descriptor::Descriptor(string type)
	: VALUE( Factory::getInstance().create(type) )
{
	HAS_DEFAULT = false;
}
Descriptor::Descriptor(string type, string val)
	: VALUE( Factory::getInstance().create(type, val) )
{
	HAS_DEFAULT = true;
}
Descriptor::Descriptor(string type, ArrayDefine def)
	: VALUE( Factory::getInstance().create(type, def) )
{
	HAS_DEFAULT = false;
}

Descriptor::Descriptor(const Descriptor& src)
	: VALUE( Factory::getInstance().copy(src.VALUE) ), HAS_DEFAULT(src.HAS_DEFAULT)
{
/*
	clog << endl << src.getTypeName() << endl;

#define PP_enum(type) \
	clog << #type << ": "; \
	try { \
		clog << static_cast< type >(VALUE); \
	} catch(const char* mes) { \
		clog << mes; \
	} \
	clog << ", "; \
	try { \
		clog << static_cast< type >(src.VALUE); \
	} catch(const char* mes) { \
		clog << mes; \
	} \
	clog << endl;

	PP_basicType(PP_enum, ;)

#undef PP_enum

	clog << "ArrayDefine" << ": ";
	try {
		clog << Encode::getInstance().demangle( static_cast< ArrayDefine >(VALUE) );
	} catch(const char* mes) {
		clog << mes;
	}
	clog << ", ";
	try {
		clog << Encode::getInstance().demangle( static_cast< ArrayDefine >(VALUE) );
	} catch(const char* mes) {
		clog << mes;
	}
	clog << endl;
 */
}

Descriptor::~Descriptor()
{
	delete &VALUE;
}

Descriptor::ArrayInfo::ArrayInfo(string type, ArrayDefine info)
	: InstanceInterface(type), ARRAY_DEFINE(info)
{
}
Descriptor::ArrayInfo::~ArrayInfo()
{
	delete &ARRAY_DEFINE;
}

#define PP_castOperator(type) \
	Descriptor::InstanceInterface::operator type() const \
	{ \
		throw runtime_error(#type " Unimplement."); \
	}

PP_basicType(PP_castOperator, )

#undef PP_castOperator

Descriptor::InstanceInterface::operator ArrayDefine() const
{
	throw runtime_error("ArrayDefine Unimplement.");
}

Descriptor::InstanceInterface& Descriptor::Factory::create(string type)
{
	return *new InstanceInterface(type);
}
Descriptor::InstanceInterface& Descriptor::Factory::create(string type, string val)
{
	return FACTORY[type].create(type, val);
}
Descriptor::InstanceInterface& Descriptor::Factory::create(string type, ArrayDefine def)
{
	return *new ArrayInfo(type, def);
}

Descriptor::InstanceInterface& Descriptor::Factory::copy(const Descriptor::InstanceInterface& src)
{
	const InstanceInterface* srcPtr = &src;

#define PP_enum(type) \
	if( typeid(*srcPtr) == typeid(InstanceFactory< type >::Value) ) { \
		return *new InstanceFactory< type >::Value( (InstanceFactory< type >::Value&)src ); \
	} else

	PP_basicType(PP_enum, ;)

#undef PP_enum

	if( typeid(*srcPtr) == typeid(ArrayInfo) ) {
		return *new ArrayInfo( src.getTypeName(), src );
	} else if( typeid(*srcPtr) == typeid(InstanceInterface) ) {
		return *new InstanceInterface(src);
	} else throw invalid_argument( Encode::getInstance().demangle(*srcPtr) );
}

Descriptor::Factory::Factory()
{
#define PP_enum(type) FACTORY.add( #type, *new InstanceFactory< type >() )

	PP_basicType(PP_enum, ;)

#undef PP_enum
}

Descriptor::Factory& Descriptor::Factory::getInstance()
{
	static Factory theFactory;
	return theFactory;
}

Element::Element(const Element& e)
	: INSTANCE( Factory::getInstance().create( *this, e.getDescriptor(), e.INSTANCE ) ),
		OWNER(e.OWNER), INDEX_1(e.INDEX_1), INDEX_2(e.INDEX_2)
{
}

Element::Element(const Descriptor& info, Binary& b)
	: INSTANCE( Factory::getInstance().create(*this, info, b) )
{
	init();
}
Element::Element(const Descriptor& info, StreamReader& f)
	: INSTANCE( Factory::getInstance().create(*this, info, f) )
{
	init();
}

Element::Element(const Array1D& owner, uint index)
	: INSTANCE( owner.getArrayDefine().exists(index)
			? Factory::getInstance().create(*this, owner.getArrayDefine()[index])
			: Factory::getInstance().create(*this)
		), INDEX_1(index)
{
	init( owner.toElement() );
}
Element::Element(const Array1D& owner, uint index , Binary& b)
	: INSTANCE( owner.getArrayDefine().exists(index)
			? Factory::getInstance().create(*this, owner.getArrayDefine()[index], b)
			: Factory::getInstance().create(*this, b)
		), INDEX_1(index)
{
	init( owner.toElement() );
}
Element::Element(const Array2D& owner, uint index1, uint index2)
	: INSTANCE( owner.getArrayDefine().exists(index2)
			? Factory::getInstance().create(*this, owner.getArrayDefine()[index2])
			: Factory::getInstance().create(*this)
		), INDEX_1(index1), INDEX_2(index2)
{
	init( owner.toElement() );
}
Element::Element(const Array2D& owner, uint index1, uint index2, Binary& b)
	: INSTANCE( owner.getArrayDefine().exists(index2)
			? Factory::getInstance().create(*this, owner.getArrayDefine()[index2], b)
			: Factory::getInstance().create(*this, b)
		), INDEX_1(index1), INDEX_2(index2)
{
	init( owner.toElement() );
}

Element::~Element()
{
	Tracer::printTrace(*this, true, cout);

	Factory::getInstance().dispose(INSTANCE);
}

#define PP_castOperator(type) \
	type Element::operator =(type src) \
	{ \
		this->substantiate(); \
		static_cast< type& >(INSTANCE) = src; \
		return src; \
	}
#define PP_castOperatorRef(type) \
	type& Element::operator =(type& src) \
	{ \
		this->substantiate(); \
		static_cast< type& >(INSTANCE) = src; \
		return src; \
	}

			PP_allType(PP_castOperator, ;)

#undef PP_castOperator
#undef PP_castOperatorRef

uint Element::getIndex1() const
{
	ArrayDefine def = getOwner().getDescriptor().getArrayDefine();
	def.exists(INDEX_1);

	return INDEX_1;
}
uint Element::getIndex2() const
{
	if( getOwner().getDescriptor().getTypeName() == "Array2D" ) return INDEX_2;
	else throw "Owner is not 2D Array.";
}

Element& Element::getOwner() const
{
	if( hasOwner() ) return *OWNER;
	else throw "Does't have owner.";
}

void Element::clear()
{
}

void Element::substantiate()
{
	INSTANCE.substantiate();

	if( hasOwner() ) {
		OWNER->substantiate();

		if( getOwner().getDescriptor().getTypeName() == "Array2D" ) {
			( (Array2D&)getOwner() )[getIndex1()].substantiate();
		}
	}
}

Element::InstanceInterface::InstanceInterface(Element& e)
	: OWNER(e), DESCRIPTOR(NULL), BIN_DATA(), EXISTS(true)
{
}
Element::InstanceInterface::InstanceInterface(Element& e, Binary& b)
	: OWNER(e), DESCRIPTOR(NULL), BIN_DATA(b), EXISTS(true)
{
}
Element::InstanceInterface::InstanceInterface(Element& e, const Descriptor& info)
	: OWNER(e), DESCRIPTOR(&info), BIN_DATA(), EXISTS(false)
{
}
Element::InstanceInterface::InstanceInterface(Element& e, const Descriptor& info, Binary& b)
	: OWNER(e), DESCRIPTOR(&info), BIN_DATA(b), EXISTS(true)
{
}
Element::InstanceInterface::InstanceInterface(Element& e, const Descriptor& info, StreamReader& s)
	: OWNER(e), DESCRIPTOR(&info), BIN_DATA(), EXISTS(true)
{
}

const Descriptor& Element::InstanceInterface::getDescriptor() const
{
	if(DESCRIPTOR != NULL) return *DESCRIPTOR;
	else throw "No Descriptor.";
}

#define PP_castOperator(type) \
	Element::InstanceInterface::operator type&() \
	{ \
		throw runtime_error(#type " Unimplement."); \
	}
#define PP_castOperatorRef(type) \
	Element::InstanceInterface::operator type&() \
	{ \
		throw runtime_error(#type " Unimplement."); \
	}

		PP_allType(PP_castOperator,)

#undef PP_castOperator
#undef PP_castOperatorRef

Element::Factory::Factory()
{
#define PP_enum(type) FACTORY.add( #type, *new InstanceFactory< type >() )
#define PP_enumRef(type) FACTORY.add( #type, *new RefInstanceFactory< type >() )

	PP_allType(PP_enum, ;)

#undef PP_enum
#undef PP_enumRef
}

Element::Factory& Element::Factory::getInstance()
{
	static Factory theFactory;
	return theFactory;
}

Element::InstanceInterface& Element::Factory::create(Element& e, const Descriptor& info)
{
	return FACTORY[info.getTypeName()].create(e, info);
}
Element::InstanceInterface& Element::Factory::create(Element& e, const Descriptor& info, Binary& b)
{
	return FACTORY[info.getTypeName()].create(e, info, b);
}
Element::InstanceInterface& Element::Factory::create(Element& e, const Descriptor& info, StreamReader& s)
{
	return FACTORY[info.getTypeName()].create(e, info, s);
}
Element::InstanceInterface& Element::Factory::create(Element& e)
{
	return *new InstanceInterface(e);
}
Element::InstanceInterface& Element::Factory::create(Element& e, Binary& b)
{
	return *new InstanceInterface(e, b);
}

void Element::Factory::dispose(InstanceInterface& i)
{
	delete &i;
}

template< > int Descriptor::Factory::InstanceFactory< int >::convert(string val)
{
	return DefineLoader::toNumber(val);
}
template< > double Descriptor::Factory::InstanceFactory< double >::convert(string val)
{
	return 0.0;
}
template< > string Descriptor::Factory::InstanceFactory< string >::convert(string val)
{
	if( (val[0] == '\"') && (val[val.length()-1] == '\"') ) {
		return val.substr( 1, val.length()-2 );
	} else throw "Cannot covert to string : " + val;
}
template< > bool Descriptor::Factory::InstanceFactory< bool >::convert(string val)
{
	return DefineLoader::toBool(val);
}
