#include "Event.hpp"


namespace rpg2k
{
	namespace structure
	{
		Instruction::Instruction()
		: code_(-1)
		{
		}
		Instruction::Instruction(StreamReader& s)
		{
			code_ = s.getBER();
			nest_ = s.getBER();

			Binary b;
			s.get(b);
			stringArgument_ = static_cast< RPG2kString >(b);

			int argNum = s.getBER();

			argument_.resize(argNum, VAR_DEF_VAL);
			for(int i = 0; i < argNum; i++) argument_[i] = s.getBER();
		}
		Instruction::Instruction(Instruction const& src)
		: code_(src.code_), nest_(src.nest_)
		, stringArgument_(src.stringArgument_), argument_(src.argument_)
		{
		}
		Instruction::~Instruction()
		{
		}

		uint Instruction::serializedSize() const
		{
			uint ret =
				getBERSize(code_) + getBERSize(nest_) +
				getBERSize( stringArgument_.size() ) + stringArgument_.size() +
				getBERSize( argument_.size() );
			for(uint i = 0; i < argument_.size(); i++) ret += getBERSize(argument_[i]);
			return ret;
		}
		void Instruction::serialize(StreamWriter& s) const
		{
			s.add( serializedSize() );

			s.setBER(code_);
			s.setBER(nest_);
			s.setBER( stringArgument_.size() ); s.write(stringArgument_);
			s.setBER( argument_.size() );
			for(uint i = 0; i < argument_.size(); i++) s.setBER(argument_[i]);
		}

		Event::Event(Binary const& b)
		{
			StreamReader s(b);
			init(s);
		}
		Event::Event(Element& e, Descriptor const& info)
		{
		}
		Event::Event(Element& e, Descriptor const& info, Binary const& b)
		{
			StreamReader s(b);
			init(s);
		}
		Event::Event(Event const& src)
		: data_(src.data_), label_(src.label_)
		{
		}
		Event::Event(Element& e, Descriptor const& info, StreamReader& s)
		{
			init(s);
		}

		void Event::init(StreamReader& s)
		{
			while( !s.eof() ) {
				Instruction inst(s);
			// check if it's label
				if(inst.code() == 12110) {
					label_.insert( std::make_pair(inst[0], data_.size() - 1) );
				}

				data_.push_back(inst);
			}

			rpg2k_assert( s.eof() );
		}

		Event::~Event()
		{
		}

		uint Event::serializedSize() const
		{
			uint ret = 0;
			for(uint i = 0; i < data_.size(); i++) ret += data_[i].serializedSize();
			return ret;
		}
		uint Event::serializedSize(uint offset) const
		{
			uint ret = 0;
			for(uint i = offset; i < data_.size(); i++) ret += data_[i].serializedSize();
			return ret;
		}
		void Event::serialize(StreamWriter& s) const
		{
			s.add( serializedSize() );

			for(uint i = 0; i < data_.size(); i++) data_[i].serialize(s);
		}

		void Event::resize(uint size)
		{
			data_.resize(size);
		}
	} // namespace structure
} // namespace rpg2k
