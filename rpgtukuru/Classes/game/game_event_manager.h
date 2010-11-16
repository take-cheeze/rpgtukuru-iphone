#pragma once

#include <kuto/kuto_error.h>
#include <kuto/kuto_irender.h>
#include <kuto/kuto_static_vector.h>
#include <kuto/kuto_task.h>

#include <boost/unordered_map.hpp>

#include <rpg2k/Define.hpp>

#include <functional>
#include <map>
#include <set>
#include <stack>
#include <utility>

class GameCharaStatus;
class GameField;
class GameMessageWindow;
class GameNameInputMenu;
class GameSaveMenu;
class GameSelectWindow;
class GameShopMenu;
class GameSkillAnime;

namespace rpg2k
{
	namespace model
	{
		class MapUnit;
		class Project;
		class SaveData;
		class DataBase;
	}
	namespace structure
	{
		class Array1D;
		class Instruction;
		class Event;
	}
}


class GameTimer : public kuto::IRender2D, public kuto::TaskCreator<GameTimer>
{
	friend class kuto::TaskCreator<GameTimer>;
public:
	void setCount(unsigned const c) { count_ = c; }
	unsigned left() const { return count_; }
	unsigned leftSecond(unsigned const frameRate = rpg2k::FRAME_PER_SECOND) const { return(count_ / frameRate); }
	unsigned leftMinute(unsigned const frameRate = rpg2k::FRAME_PER_SECOND) const { return leftSecond(frameRate) / 60; }

private:
	GameTimer()
	: IRender2D(kuto::Layer::OBJECT_2D, 0.f)
	{
	}

	virtual void update() { if(count_) { --count_; } else { pauseUpdate(); } }
	virtual void render(kuto::Graphics2D& g) const
	{
		// TODO
	}

private:
	unsigned count_;
	// TODO: battle
};

#define DEFUALT_TIMER_NUMBER 2

class GameEventManager : public kuto::Task, public kuto::TaskCreatorParam1<GameEventManager, GameField&>
{
	friend class TaskCreatorParam1<GameEventManager, GameField&>;
private:
	GameEventManager(GameField& f);

	bool execute(rpg2k::structure::Instruction const& inst);

	class Context
	{
	public:
		typedef unsigned Pointer;
		typedef unsigned Nest;

		Context(GameEventManager& owner, unsigned evID, rpg2k::EventStart::Type t);
		~Context();

		void skipToEndOfJunction(unsigned nest, unsigned code);
		bool skipToElse(unsigned nest, unsigned code);

		Pointer const& pointer() const { return eventStack_.top().second; }
		Pointer& pointer() { return eventStack_.top().second; }
		void setPointer(Pointer const& p) { eventStack_.top().second = p; }
		void incrementPointer() { ++eventStack_.top().second; }
		rpg2k::structure::Event const& event() const { return *eventStack_.top().first; }

		bool isWaiting() const;
		void start(rpg2k::structure::Event const& ev);
		void call(rpg2k::structure::Event const& ev, Pointer const& p = 0);
		void ret(); // return

		void clearCallStack();

		unsigned eventID() const { return eventID_; }

		GameTimer& waiter() { return waiter_; }

		rpg2k::EventStart::Type startType() const { return type_; }

		void startLoop(Nest const& n);
		void continueLoop();
		void breakLoop();

		void update();

		bool stackEmpty() const { return eventStack_.empty(); }

	private:
		GameEventManager& owner_;
		GameTimer& waiter_;
		unsigned const eventID_;
		rpg2k::EventStart::Type const type_;
		std::stack< std::pair<rpg2k::structure::Event const*, Pointer> > eventStack_;
		std::stack< std::pair<Nest, Pointer> > loopStack_;
	}; // class Context

	void setCurrent(Context& cont);
	void waitProcess(Context& cont);

private:
	GameField& field_;
	GameMessageWindow& messageWindow_;
	GameNameInputMenu& nameInputMenu_;
	GameSaveMenu& saveMenu_;
	GameSelectWindow& selectWindow_;
	GameShopMenu& shopMenu_;
	GameSkillAnime* skillAnime_;

	std::deque<GameTimer*> timer_;
	typedef void(GameEventManager::*Command)(rpg2k::structure::Instruction const&);
	typedef boost::unordered_map<unsigned, Command> CommandTable;
	CommandTable commandTable_, commandWaitTable_;

	std::set<unsigned> branchCode_;

	GameTimer& waiter_;
	unsigned stepCounter_; // limits executing command in one loop

	Context* current_;
	typedef std::multimap< rpg2k::EventStart::Type, Context*, std::greater<rpg2k::EventStart::Type> > ContextList;
	ContextList contextList_;

	bool eventLeft_;

	struct
	{
		rpg2k::model::Project* project;
		rpg2k::model::DataBase* ldb;
		rpg2k::model::MapUnit* lmu;
		rpg2k::model::SaveData* lsd;
	} cache_;

public:
	bool isWaiting() const;

private:
	void setWait(bool b);
	void setWaitCount(unsigned c);

	void addContext(unsigned evID, rpg2k::structure::Event const& ev, rpg2k::EventStart::Type t);
	void updateCommonContext();
	void updateMapContext();

	template<unsigned CODE> void command(rpg2k::structure::Instruction const&);
	template<unsigned CODE> void commandWait(rpg2k::structure::Instruction const&);
	template<unsigned CODE> void addCommand();
	template<unsigned CODE> void addCommandWait();
	void initCommandTable();

	typedef kuto::StaticVector<unsigned, rpg2k::MEMBER_MAX> TargetCharacter;
	struct Target { enum Type { PARTY = 0, IMMEDIATE = 1, VARIABLE = 2, }; }; 
	TargetCharacter targetCharacter(Target::Type t, unsigned val) const;

	virtual void update();

	void addLevelUpMessage(unsigned charID, int oldLevel);
	void openGameMassageWindow();
	void openGameSelectWindow();
}; // class GameEventManager
