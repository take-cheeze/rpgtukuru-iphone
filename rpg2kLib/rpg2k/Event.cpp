#include "Event.hpp"
#include "Stream.hpp"


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
			code_ = s.ber();
			nest_ = s.ber();

			Binary b;
			s.get(b);
			stringArgument_ = static_cast<RPG2kString>(b);

			int argNum = s.ber();

			argument_.resize(argNum, VAR_DEF_VAL);
			for(int i = 0; i < argNum; i++) argument_[i] = s.ber();
		}
		Instruction::Instruction(Instruction const& src)
		: code_(src.code_), nest_(src.nest_)
		, stringArgument_(src.stringArgument_), argument_(src.argument_)
		{
		}

		int32_t Instruction::at(unsigned index) const
		{ rpg2k_assert( index < argument_.size() ); return argument_[index]; }
		int32_t Instruction::operator [](unsigned index) const
		{ rpg2k_assert( index < argument_.size() ); return argument_[index]; }
		int32_t& Instruction::at(unsigned index)
		{ rpg2k_assert( index < argument_.size() ); return argument_[index]; }
		int32_t& Instruction::operator [](unsigned index)
		{ rpg2k_assert( index < argument_.size() ); return argument_[index]; }

		unsigned Instruction::serializedSize() const
		{
			unsigned ret =
				berSize(code_) + berSize(nest_) +
				berSize( stringArgument_.size() ) + stringArgument_.size() +
				berSize( argument_.size() );
			for(unsigned i = 0; i < argument_.size(); i++) ret += berSize(argument_[i]);
			return ret;
		}
		void Instruction::serialize(StreamWriter& s) const
		{
			s.add( serializedSize() );

			s.setBER(code_);
			s.setBER(nest_);
			s.setBER( stringArgument_.size() ); s.write(stringArgument_);
			s.setBER( argument_.size() );
			for(unsigned i = 0; i < argument_.size(); i++) s.setBER(argument_[i]);
		}

		Event::Event(Binary const& b)
		{
			StreamReader s( std::auto_ptr<StreamInterface>( new BinaryReaderNoCopy(b) ) );
			init(s);
		}

		void Event::init(StreamReader& s)
		{
			while( !s.eof() ) {
				data_.push_back( Instruction(s) );
				if(data_.back().code() == 12110) { // check for label
					if( !label_.insert( std::make_pair(data_.back()[0], data_.size() - 1) ).second ) {
						rpg2k_assert(false);
					}
				}
			}
		}

		unsigned Event::serializedSize() const
		{
			unsigned ret = 0;
			for(unsigned i = 0; i < data_.size(); i++) ret += data_[i].serializedSize();
			return ret;
		}
		unsigned Event::serializedSize(unsigned offset) const
		{
			unsigned ret = 0;
			for(unsigned i = offset; i < data_.size(); i++) ret += data_[i].serializedSize();
			return ret;
		}
		void Event::serialize(StreamWriter& s) const
		{
			s.add( serializedSize() );

			for(unsigned i = 0; i < data_.size(); i++) data_[i].serialize(s);
		}

		void Event::resize(unsigned size)
		{
			data_.resize(size);
		}
	} // namespace structure
} // namespace rpg2k
