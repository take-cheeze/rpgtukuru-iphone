#ifndef _INC__RPG2K__MODEL__EXECUTE_HPP
#define _INC__RPG2K__MODEL__EXECUTE_HPP

#include <map>
#include <set>
#include <stack>
#include <vector>


namespace rpg2k
{
	class Main;

	namespace structure
	{
		class Event;
		class Instruction;
	}

	namespace model
	{
		class Project;

		class Execute
		{
		public:
			enum State { SUCCESS, FAILED, NOT_IMPLEMENTED, NEXT_FRAME, };
		private:
			Main& owner_;

			enum EventType { EV_COMMON, EV_MAP, EV_BATTLE, EV_TYPE_END };
			struct ExecInfo
			{
				EventType type;
				uint id;

				uint counter;
				bool end;

				EventStart::Type start;
				uint page;
				structure::Event* event;
			};

			std::vector< std::map< uint, ExecInfo > > execInfo_;
			std::map< uint, uint > pageMap_;

			std::vector< std::map< uint, std::multimap< uint, uint > > >* eventMap_;
			std::map< uint, uint >* pageNo_;
			std::stack< ExecInfo > eventStack_;
		protected:
			Project& getProject();
			Main& getOwner() { return owner_; }

			static const int SKIP_CURRENT = -1;
			void skipToEndOfJunction(int nest = SKIP_CURRENT, int code = CODE__END_OF_JUNCTION);
			bool skipToElse(int nest = SKIP_CURRENT, int code = SKIP_CURRENT);

			bool nextEvent();

			bool isJunction(uint code) const;

			bool hasExec() const { return !eventStack_.empty(); }
			ExecInfo& curExec() { return eventStack_.top(); }
			ExecInfo const& curExec() const { return eventStack_.top(); }
			void pushExec(ExecInfo const&  exe) { eventStack_.push(exe); }
			void  popExec() { eventStack_.pop(); }

			static const uint CODE__END_OF_JUNCTION = 10;
		public:
			Execute(Main& m);

			const structure::Instruction* nextInst();
			const structure::Instruction* currentInst() const;

			void refresh(
				std::vector< std::map< uint, std::multimap< uint, uint > > >& eventMap,
				std::map< uint, uint >& pageNo
			);

			Execute::State execute(structure::Instruction const& inst);
			Execute::State executeBattle(structure::Instruction const& inst);
		private:
			class CommandInterface
			{
			private:
				Execute* owner_;
			protected:
				Execute& getOwner() const { return *owner_; }
			public:
				CommandInterface() {}
				CommandInterface(Execute& e) : owner_(&e) {}

				virtual Execute::State operator()(
					RPG2kString const& strArg,
					std::vector< int32_t > const& args
				) const { rpg2k_assert(false); }
			};

			std::map< uint, CommandInterface > commandTable_;
			std::map< uint, CommandInterface > battleCommandTable_;

			template< uint Code >
			class Command : public CommandInterface
			{
			public:
				Command() {}
				Command(Execute& e) : CommandInterface(e) {}
				virtual Execute::State operator()(
					RPG2kString const& strArg,
					std::vector< int32_t > const& args
				) const;
				uint code() { return Code; }
			};
		}; // class Execute
	} // namespace model
} // namespace rpg2k

#endif
