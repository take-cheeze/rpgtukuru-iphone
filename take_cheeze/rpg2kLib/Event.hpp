#ifndef _INC__RPG2k__EVENT_HPP
#define _INC__RPG2k__EVENT_HPP

#include <vector>

#include "Struct.hpp"
#include "Stream.hpp"

namespace rpg2kLib
{
	namespace structure
	{

		class Instruction
		{
		private:
			Binary BIN_DATA;

			uint CODE;
			uint NEST;

			std::string STRING;

			std::vector< int32_t > ARGS;
		public:
			Instruction();
			Instruction(StreamReader& s);

			virtual ~Instruction();

			uint code() const { return CODE; }
			void setCode(uint c) { CODE = c; }

			uint nest() const { return NEST; }
			void setNest(uint n) { NEST = n; }

			std::string getString() const { return STRING; }
			void setString(std::string str) { STRING = str; }
			operator std::string() const { return STRING; }

			int32_t operator [](uint index) const { return ARGS.at(index); }
			uint getArgNum() const { return ARGS.size(); }
			void addArg(int32_t arg) { ARGS.push_back(arg); }

			uint getSize();
			operator const Binary&();
		};

		class Element;
		class Descriptor;

		class Event
		{
		private:
			Binary BIN_DATA;

			// vector< Instruction* > DATA;
			std::vector< Instruction > DATA;

			std::map< uint, uint > LABEL;
		protected:
			void init(StreamReader& s);

			Event();
		public:
			Event(Element& e, const Descriptor& info);
			Event(Element& e, const Descriptor& info, StreamReader& s);
			Event(Element& e, const Descriptor& info, Binary& b);

			Event(/* uint length, */ Binary& b);

			virtual ~Event();

			uint length() { return DATA.size(); }

			const Instruction& operator [](uint index) const { return DATA.at(index); }
			uint label(uint num);

			uint instNum() { return DATA.size(); }
			void resize(uint num);

			uint getSize(uint offset = 0);

			const Binary& toBinary();
		};

	};
};

#endif
