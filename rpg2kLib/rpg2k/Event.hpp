#ifndef _INC__RPG2K__EVENT_HPP
#define _INC__RPG2K__EVENT_HPP

#include "Define.hpp"

#include <deque>
#include <map>
#include <memory>
#include <vector>


namespace rpg2k
{
	class Binary;

	namespace structure
	{
		class StreamReader;
		class StreamWriter;

		class Instruction
		{
		private:
			unsigned code_, nest_;

			RPG2kString stringArgument_;
			std::vector<int32_t> argument_;
		public:
			Instruction();
			Instruction(StreamReader& s);
			Instruction(Instruction const& src);

			unsigned code() const { return code_; }
			void setCode(unsigned c) { code_ = c; }

			unsigned nest() const { return nest_; }
			void setNest(unsigned n) { nest_ = n; }

			RPG2kString const& string() const { return stringArgument_; }
			void setString(RPG2kString const& str) { stringArgument_ = str; }
			operator RPG2kString const&() const { return stringArgument_; }

			std::vector<int32_t> const& args() const { return argument_; }
			std::vector<int32_t> args() { return argument_; }

			int32_t at(unsigned index) const;
			int32_t operator [](unsigned index) const;
			int32_t& at(unsigned index);
			int32_t& operator [](unsigned index);

			unsigned argNum() const { return argument_.size(); }
			void addArg(int32_t arg) { argument_.push_back(arg); }

			unsigned serializedSize() const;
			void serialize(StreamWriter& s) const;
		}; // class Instruction

		class Element;
		class Descriptor;

		class Event
		{
		protected:
			void init(StreamReader& s);
		public:
			Event() {}
			Event(Binary const& b);

			Instruction const& operator [](unsigned index) const { return data_[index]; }

			unsigned size() const { return data_.size(); }
			void resize(unsigned num);

			unsigned serializedSize() const;
			unsigned serializedSize(unsigned offset) const;
			void serialize(StreamWriter& s) const;

			typedef std::deque<Instruction> Data;
			// Data const& data() const { return data_; }

			typedef std::map<unsigned, unsigned> LabelTable;
			LabelTable const& labelTable() const { return label_; }
		private:
			Data data_;
			LabelTable label_;
		}; // class Event
	} // namespace structure
} // namespace rpg2k

#endif
