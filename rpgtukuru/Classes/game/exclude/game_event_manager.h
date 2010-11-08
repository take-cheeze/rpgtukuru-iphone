/**
 * @file
 * @brief Game Event Manager
 * @author project.kuto
 */
#pragma once

#include <map>

#include <kuto/kuto_task.h>
#include <kuto/kuto_simple_array.h>
#include <kuto/kuto_array.h>
#include <kuto/kuto_static_stack.h>

#include <deque>
#include <map>
#include <stack>
#include <utility>
#include <vector>

#include <rpg2k/Event.hpp>

class GameBgm;
class GameCharaStatus;
class GameEventMapChip;
class GameEventPicture;
class GameField;
class GameMessageWindow;
class GameNameInputMenu;
class GameNpc;
class GameSaveMenu;
class GameSelectWindow;
class GameShopMenu;
class GameSkillAnime;

namespace rpg2k { namespace structure { class Event; } }

static const uint LABEL_MAX = 100;
// at "rpg maker 2000 value" or later
static const uint PICTURE_MAX = 50;

class GameEventManager : public kuto::Task, public kuto::TaskCreatorParam1<GameEventManager, GameField*>
{
	friend class kuto::TaskCreatorParam1<GameEventManager, GameField*>;
public:
	struct PageInfo {
		int			index;
		GameNpc*	npc;
		GameEventMapChip*	mapChip;
		kuto::u32	npcCrc;
		int			x;
		int			y;
		bool		cleared;

		PageInfo() : index(0), npc(NULL), mapChip(NULL), npcCrc(0), x(0), y(0), cleared(false) {}
	};
	struct TimerInfo {
		int		count;
		bool	enable;
		bool	enableBattle;
		bool	draw;

		TimerInfo() : count(0), enable(false), enableBattle(false), draw(false) {}
	};
	typedef std::vector<TimerInfo> TimerList;
	struct MessageWindowSetting {
		enum PosType {
			kPosTypeUp,
			kPosTypeCenter,
			kPosTypeDown,
		} pos;
		bool	showFrame;
		bool	autoMove;
		bool	enableOtherEvent;

		MessageWindowSetting() : pos(kPosTypeDown), showFrame(true), autoMove(false), enableOtherEvent(false) {}
	};
	struct ConditionInfo {
		int					nest;
		int					value;

		ConditionInfo() : nest(0), value(0) {}
		ConditionInfo(int nest, int value) : nest(nest), value(value) {}
	};
	struct LoopInfo {
		int					startIndex;
		int					endIndex;
		int					conditionSize;

		LoopInfo() : startIndex(0), endIndex(0), conditionSize(0) {}
	};
	typedef kuto::StaticStack<ConditionInfo, 64> ConditionStack;
	typedef kuto::StaticStack<LoopInfo, 64> LoopStack;
	struct WaitEventInfo {
		bool					enable;
		int						eventIndex;
		const rpg2k::structure::Event*	page;
		int						pos;
		int						nextPos;
		int						count;
		bool					executeChildCommands;
		ConditionStack			conditionStack;
		LoopStack				loopStack;

		WaitEventInfo() : enable(false), page(NULL), pos(0), nextPos(0), count(0) {}
	};
	struct RestEventInfo {
		bool					enable;
		int						eventIndex;
		rpg2k::structure::Event			eventListCopy;
		int						pos;
		int						nextPos;
		int						count;
		bool					executeChildCommands;
		ConditionStack			conditionStack;
		LoopStack				loopStack;

		RestEventInfo() : enable(false), pos(0), nextPos(0), count(0) {}
	};
	struct CallEventInfo : public WaitEventInfo {
		kuto::Array<int, LABEL_MAX>		labels;
	};
	typedef kuto::StaticStack<CallEventInfo, 64> CallEventStack;
	typedef void(GameEventManager::*ComFunc)(const rpg2k::structure::Instruction&);
	typedef std::map<int, ComFunc> ComFuncMap;

	void preMapChange();
	void postMapChange();

	std::deque< std::map< uint, std::multimap<uint, uint> > > const& eventMap() { return eventMap_; }

private:
	GameEventManager(GameField* field);

	virtual bool initialize();
	virtual void update();
	virtual void draw();

	void updateEventAppear();
	void updateEvent();
	void updateWaitEvent();
	void updateEncount();
	void executeCommands(const rpg2k::structure::Event& eventList, int start);
	// std::string getEncountBattleMap(const MapTree::MapInfo& mapInfo, int terrainId);
	void openGameMassageWindow();
	void openGameSelectWindow();
	void addLevelUpMessage(const GameCharaStatus& status, int oldLevel);
	void initEventPageInfos();
	// bool isEventConditionOk(const CRpgEventCondition& condition);
	void restoreCallStack();

	template< uint CODE >
	void command(const rpg2k::structure::Instruction& com);
	template< uint CODE >
	void commandWait(const rpg2k::structure::Instruction& com);

	void commandDummy(const rpg2k::structure::Instruction&);
	void commandWaitDummy(const rpg2k::structure::Instruction&);

protected:
	template<int CODE> void addCommand();
	template<int CODE> void addCommandWait();

private:
	GameField*					gameField_;
	GameMessageWindow*			gameMessageWindow_;
	GameSelectWindow*			selectWindow_;
	GameNameInputMenu*			nameInputMenu_;
	GameShopMenu*				shopMenu_;
	GameSaveMenu*				saveMenu_;

	MessageWindowSetting		messageWindowSetting_;
	// kuto::SimpleArray<PageInfo>	eventPageInfos_;
	ComFuncMap					comFuncMap_;
	ComFuncMap					comWaitFuncMap_;
	kuto::Array<int, LABEL_MAX>		labels_;
	// int							currentEventIndex_;
	// const rpg2k::structure::Event*		currentEventPage_;
	unsigned int				currentCommandIndex_;
	WaitEventInfo				waitEventInfo_;
	bool						backupWaitInfoEnable_;
	bool						executeChildCommands_;
	bool						startDecideButton_;
	int							encountStep_;
	ConditionStack				conditionStack_;
	LoopStack					loopStack_;
	kuto::Array<GameEventPicture*, 50>		pictures_;
	RestEventInfo				restEventInfo_;
	// GameChara*					routeSetChara_;
	CallEventStack				callStack_;
	GameBgm*					bgm_;
	GameSkillAnime*				skillAnime_;
	TimerList					timer_;

	enum { EV_COMMON, EV_MAP, EV_BATTLE, EV_TYPE_END };
	struct ExecInfo
	{
		int type;
		uint id;

		uint pointer;
		bool end;

		rpg2k::EventStart::Type start;
		uint page;
		rpg2k::structure::Event* event;
	};
	std::stack<ExecInfo> eventStack_;
	std::deque< std::map< uint, ExecInfo > > execInfo_;
	std::map<uint, uint> pageMap_;
	std::stack<uint> touchFromParty_, touchFromEvent_;
	std::deque< std::map< uint, std::multimap<uint, uint> > > eventMap_; // (priority, y, x)
	std::map<uint, uint> pageNum_;
};
