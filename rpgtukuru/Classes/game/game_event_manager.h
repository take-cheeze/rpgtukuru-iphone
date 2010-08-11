/**
 * @file
 * @brief Game Event Manager
 * @author project.kuto
 */
#pragma once

#include <map>

#include "game_event_code_define.h"

#include <kuto/kuto_task.h>
#include <kuto/kuto_simple_array.h>
#include <kuto/kuto_array.h>
#include <kuto/kuto_static_stack.h>
#include <rpg2k/MapUnit.hpp>
#include <rpg2k/MapTree.hpp>

class GameField;
class GameMessageWindow;
class GameSelectWindow;
class GameChara;
class GameNpc;
class GameEventPicture;
class GameCharaStatus;
class GameNameInputMenu;
class GameShopMenu;
class GameBgm;
class GameEventMapChip;
class GameSkillAnime;

static const uint LABEL_MAX = 100;
// at "rpg maker 2000 value" or later
static const uint PICTURE_MAX = 50;

class GameEventManager : public kuto::Task
{
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

	static GameEventManager* createTask(kuto::Task* parent, GameField* field) { return new GameEventManager(parent, field); }

	void preMapChange();
	void postMapChange();
private:
	GameEventManager(kuto::Task* parent, GameField* field);

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
	GameChara* getCharaFromEventId(int eventId);
	void restoreCallStack();

	template< uint CODE >
	void command(const rpg2k::structure::Instruction& com);
	template< uint CODE >
	void commandWait(const rpg2k::structure::Instruction& com);

private:
	GameField*					gameField_;
	GameMessageWindow*			gameMessageWindow_;
	GameSelectWindow*			selectWindow_;
	GameNameInputMenu*			nameInputMenu_;
	GameShopMenu*				shopMenu_;
	MessageWindowSetting		messageWindowSetting_;
	kuto::SimpleArray<PageInfo>	eventPageInfos_;
	ComFuncMap					comFuncMap_;
	ComFuncMap					comWaitFuncMap_;
	TimerInfo					timer_;
	kuto::Array<int, LABEL_MAX>		labels_;
	int							currentEventIndex_;
	const rpg2k::structure::Event*		currentEventPage_;
	int							currentCommandIndex_;
	WaitEventInfo				waitEventInfo_;
	bool						backupWaitInfoEnable_;
	bool						executeChildCommands_;
	bool						startDecideButton_;
	int							encountStep_;
	ConditionStack				conditionStack_;
	LoopStack					loopStack_;
	kuto::Array<GameEventPicture*, 50>		pictures_;
	RestEventInfo				restEventInfo_;
	GameChara*					routeSetChara_;
	CallEventStack				callStack_;
	GameBgm*					bgm_;
	GameSkillAnime*				skillAnime_;
};

#define PP_protoType(code) \
	template< > void GameEventManager::command<code>(const rpg2k::structure::Instruction& com)
PP_protoType(CODE_OPERATE_SWITCH);
PP_protoType(CODE_OPERATE_VAR);
PP_protoType(CODE_OPERATE_ITEM);
PP_protoType(CODE_OPERATE_MONEY);
PP_protoType(CODE_OPERATE_TIMER);
PP_protoType(CODE_GOTO_LABEL);
PP_protoType(CODE_PARTY_CHANGE);
PP_protoType(CODE_LOCATE_MOVE);
PP_protoType(CODE_LOCATE_SAVE);
PP_protoType(CODE_LOCATE_LOAD);
PP_protoType(CODE_TXT_SHOW);
PP_protoType(CODE_TXT_OPTION);
PP_protoType(CODE_TXT_FACE);
PP_protoType(CODE_BTL_GO_START);
PP_protoType(CODE_BTL_GO_WIN);
PP_protoType(CODE_BTL_GO_ESCAPE);
PP_protoType(CODE_BTL_GO_LOSE);
PP_protoType(CODE_BLOCK_END);
PP_protoType(CODE_IF_START);
PP_protoType(CODE_IF_ELSE);
PP_protoType(CODE_SELECT_START);
PP_protoType(CODE_SELECT_CASE);
PP_protoType(CODE_GAMEOVER);
PP_protoType(CODE_TITLE);
PP_protoType(CODE_EVENT_BREAK);
PP_protoType(CODE_EVENT_CLEAR);
PP_protoType(CODE_LOOP_START);
PP_protoType(CODE_LOOP_BREAK);
PP_protoType(CODE_LOOP_END);
PP_protoType(CODE_WAIT);
PP_protoType(CODE_PICT_SHOW);
PP_protoType(CODE_PICT_MOVE);
PP_protoType(CODE_PICT_CLEAR);
PP_protoType(CODE_SYSTEM_SCREEN);
PP_protoType(CODE_SCREEN_CLEAR);
PP_protoType(CODE_SCREEN_SHOW);
PP_protoType(CODE_SCREEN_SCROLL);
PP_protoType(CODE_CHARA_TRANS);
PP_protoType(CODE_PARTY_REFRESH);
PP_protoType(CODE_PARTY_EXP);
PP_protoType(CODE_PARTY_LV);
PP_protoType(CODE_PARTY_POWER);
PP_protoType(CODE_PARTY_SKILL);
PP_protoType(CODE_EVENT_GOSUB);
PP_protoType(CODE_CHARA_MOVE);
PP_protoType(CODE_MOVEALL_START);
PP_protoType(CODE_MOVEALL_CANSEL);
PP_protoType(CODE_NAME_INPUT);
PP_protoType(CODE_PARTY_NAME);
PP_protoType(CODE_PARTY_TITLE);
PP_protoType(CODE_PARTY_WALK);
PP_protoType(CODE_PARTY_FACE);
PP_protoType(CODE_MM_BGM_PLAY);
PP_protoType(CODE_OPERATE_KEY);
PP_protoType(CODE_PANORAMA);
PP_protoType(CODE_INN);
PP_protoType(CODE_INN_IF_START);
PP_protoType(CODE_INN_IF_ELSE);
PP_protoType(CODE_SHOP);
PP_protoType(CODE_MM_SOUND);
PP_protoType(CODE_SCREEN_COLOR);
PP_protoType(CODE_BTLANIME);
PP_protoType(CODE_PARTY_SOUBI);
PP_protoType(CODE_IF_END);

#define PP_protoTypeWait(code) \
	template< > void GameEventManager::commandWait<code>(const rpg2k::structure::Instruction& com)
PP_protoTypeWait(CODE_LOCATE_MOVE);
PP_protoTypeWait(CODE_TXT_SHOW);
PP_protoTypeWait(CODE_BTL_GO_START);
PP_protoTypeWait(CODE_SELECT_START);
PP_protoTypeWait(CODE_WAIT);
PP_protoTypeWait(CODE_PICT_MOVE);
PP_protoTypeWait(CODE_SCREEN_SCROLL);
PP_protoTypeWait(CODE_NAME_INPUT);
PP_protoTypeWait(CODE_OPERATE_KEY);
PP_protoTypeWait(CODE_INN);
PP_protoTypeWait(CODE_SHOP);
PP_protoTypeWait(CODE_SCREEN_COLOR);
PP_protoTypeWait(CODE_BTLANIME);
