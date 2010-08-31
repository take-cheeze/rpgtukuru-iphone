#ifndef _INC__RPG2K__EVENT_HPP
#define _INC__RPG2K__EVENT_HPP

#include <vector>
#include <deque>

#include "Stream.hpp"


namespace rpg2k
{
	namespace structure
	{
		class Instruction
		{
		private:
			uint code_, nest_;

			RPG2kString stringArgument_;
			std::vector< int32_t > argument_;
		public:
			Instruction();
			Instruction(StreamReader& s);
			Instruction(Instruction const& src);

			~Instruction();

			uint code() const { return code_; }
			void setCode(uint c) { code_ = c; }

			uint nest() const { return nest_; }
			void setNest(uint n) { nest_ = n; }

			RPG2kString const& getString() const { return stringArgument_; }
			void setString(RPG2kString const& str) { stringArgument_ = str; }
			operator RPG2kString const&() const { return stringArgument_; }

			std::vector< int32_t > const& getArgs() const { return argument_; }
			std::vector< int32_t > getArgs() { return argument_; }

			int32_t at(uint index) const { return argument_.at(index); }
			int32_t operator [](uint index) const { return argument_[index]; }
			int32_t& at(uint index) { return argument_.at(index); }
			int32_t& operator [](uint index) { return argument_[index]; }

			uint getArgNum() const { return argument_.size(); }
			void addArg(int32_t arg) { argument_.push_back(arg); }

			uint serializedSize() const;
			void serialize(StreamWriter& s) const;
		}; // class Instruction

		class Element;
		class Descriptor;

		class Event
		{
		private:
			std::deque< Instruction > data_;
			std::map< uint, uint > label_;
		protected:
			void init(StreamReader& s);
		public:
			Event(Element& e, Descriptor const& info);
			Event(Element& e, Descriptor const& info, StreamReader& s);
			Event(Element& e, Descriptor const& info, Binary const& b);

			Event() {}
			Event(Binary const& b);
			Event(Event const& src);

			~Event();

			uint size() const { return data_.size(); }

			Instruction const& operator [](uint index) const { return data_[index]; }
			uint label(uint num);

			uint instNum() const { return data_.size(); }
			void resize(uint num);

			uint serializedSize() const;
			uint serializedSize(uint offset) const;
			void serialize(StreamWriter& s) const;
		}; // class Event
	} // namespace structure
} // namespace rpg2k

#endif
