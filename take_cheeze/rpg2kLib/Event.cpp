#include "Event.hpp"

using namespace rpg2kLib::structure;

Instruction::Instruction()
	: BIN_DATA(),
		CODE(-1), NEST(0), STRING(""), ARGS()
{
}
Instruction::Instruction(StreamReader& s)
	: BIN_DATA(), ARGS()
{
	CODE = s.getBER();
	NEST = s.getBER();

	Binary b;
	s >= b;
	STRING = static_cast< std::string >(b);

	int argNum = s.getBER();

	ARGS.resize(argNum, VAR_DEF_VAL);
	for(int i = 0; i < argNum; i++) s >> ARGS[i];
}
Instruction::~Instruction()
{
}

uint Instruction::getSize()
{
	uint ret =
		getBERSize(CODE) + getBERSize(NEST) +
		getBERSize( STRING.size() ) + STRING.size() +
		getBERSize( ARGS.size() );
	for(uint i = 0; i < ARGS.size(); i++) ret += getBERSize(ARGS[i]);
	return ret;
}
Instruction::operator const Binary&()
{
	BIN_DATA.reset( getSize() );
	StreamWriter s(BIN_DATA);

	s << CODE << NEST << STRING.size() << STRING << ARGS.size();
	for(uint i = 0; i < ARGS.size(); i++) s.setBER(ARGS[i]);

	return BIN_DATA;
}

Event::Event() : BIN_DATA()
{
	StreamReader s(BIN_DATA);
	init(s);
}
Event::Event(/* uint length, */ Binary& b)
{
	StreamReader s(b);
	init(s);
}
Event::Event(Element& e, const Descriptor& info) : BIN_DATA()
{
	StreamReader s(BIN_DATA);
	init(s);
}
Event::Event(Element& e, const Descriptor& info, Binary& b)
{
	StreamReader s(b);
	init(s);
}
Event::Event(Element& e, const Descriptor& info, StreamReader& s) { init(s); }

void Event::init(StreamReader& s)
{
	while( !s.eof() ) {
		DATA.push_back( Instruction(s) );

		const Instruction& inst = *DATA.end();

		// check if it's label
		if(inst.code() == 12110)
			LABEL.insert( std::map< uint, uint >::value_type(inst[0], DATA.size() - 1) );
	}
}

Event::~Event()
{
}

uint Event::getSize(uint offset)
{
	uint ret = 0;
	for(uint i = offset; i < DATA.size(); i++) ret += DATA[i].getSize();
	return ret;
}
const Binary& Event::toBinary()
{
	BIN_DATA.reset( getSize() );
	StreamWriter s(BIN_DATA);

	for(uint i = 0; i < DATA.size(); i++) s << DATA[i];

	return BIN_DATA;
}

void Event::resize(uint size)
{
	DATA.resize(size);
}
