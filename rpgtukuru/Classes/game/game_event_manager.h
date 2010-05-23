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
#include "CRpgLmu.h"
#include "CRpgLmt.h"

class GameField;
class GameMessageWindow;
class GameSelectWindow;
class GameChara;
class GameNpc;
class GameEventPicture;
class GameCharaStatus;
class GameNameInputMenu;
class GameBgm;


class GameEventManager : public kuto::Task
{
public:
	typedef CRpgLmu::MAPEVENT::MAPEVENT_PAGE EventPage;
	struct PageInfo {
		int			index;
		GameNpc*	npc;
		u32			npcCrc;
		int			x;
		int			y;
		bool		cleared;
		
		PageInfo() : index(0), npc(NULL), npcCrc(0), x(0), y(0), cleared(false) {}
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
		};
		PosType	pos;
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
		const CRpgEventList*	page;
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
		CRpgEventList			eventListCopy;
		int						pos;
		int						nextPos;
		int						count;
		bool					executeChildCommands;
		ConditionStack			conditionStack;
		LoopStack				loopStack;
		
		RestEventInfo() : enable(false), pos(0), nextPos(0), count(0) {}
	};
	struct CallEventInfo : public WaitEventInfo {
		kuto::Array<int, 100>		labels;
	};
	typedef kuto::StaticStack<CallEventInfo, 64> CallEventStack;
	typedef void(GameEventManager::*ComFunc)(const CRpgEvent&);
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
	void executeCommands(const CRpgEventList& eventList, int start);
	std::string getEncountBattleMap(const CRpgLmt::MapInfo& mapInfo, int terrainId);
	void openGameMassageWindow();
	void addLevelUpMessage(const GameCharaStatus& status, int oldLevel);
	void initEventPageInfos();
	bool isEventConditionOk(const CRpgEventCondition& condition);
	GameChara* getCharaFromEventId(int eventId);
	void restoreCallStack();
	
	void comOperateSwitch(const CRpgEvent& com);
	void comOperateVar(const CRpgEvent& com);
	void comOperateItem(const CRpgEvent& com);
	void comOperateMoney(const CRpgEvent& com);
	void comOperateTimer(const CRpgEvent& com);
	void comOperateJumpLabel(const CRpgEvent& com);
	void comOperatePartyChange(const CRpgEvent& com);
	void comOperateLocateMove(const CRpgEvent& com);
	void comOperateLocateSave(const CRpgEvent& com);
	void comOperateLocateLoad(const CRpgEvent& com);
	void comOperateTextShow(const CRpgEvent& com);
	void comOperateTextOption(const CRpgEvent& com);
	void comOperateTextFace(const CRpgEvent& com);
	void comOperateBattleStart(const CRpgEvent& com);
	void comOperateBattleWin(const CRpgEvent& com);
	void comOperateBattleEscape(const CRpgEvent& com);
	void comOperateBattleLose(const CRpgEvent& com);
	void comOperateBranchEnd(const CRpgEvent& com);
	void comOperateIfStart(const CRpgEvent& com);
	void comOperateIfElse(const CRpgEvent& com);
	void comOperateSelectStart(const CRpgEvent& com);
	void comOperateSelectCase(const CRpgEvent& com);
	void comOperateGameOver(const CRpgEvent& com);
	void comOperateReturnTitle(const CRpgEvent& com);
	void comOperateEventBreak(const CRpgEvent& com);
	void comOperateEventClear(const CRpgEvent& com);
	void comOperateLoopStart(const CRpgEvent& com);
	void comOperateLoopBreak(const CRpgEvent& com);
	void comOperateLoopEnd(const CRpgEvent& com);
	void comOperateWait(const CRpgEvent& com);
	void comOperatePictureShow(const CRpgEvent& com);
	void comOperatePictureMove(const CRpgEvent& com);
	void comOperatePictureClear(const CRpgEvent& com);
	void comOperateFadeType(const CRpgEvent& com);
	void comOperateFadeOut(const CRpgEvent& com);
	void comOperateFadeIn(const CRpgEvent& com);
	void comOperateMapScroll(const CRpgEvent& com);
	void comOperatePlayerVisible(const CRpgEvent& com);
	void comOperatePlayerCure(const CRpgEvent& com);
	void comOperateAddExp(const CRpgEvent& com);
	void comOperateAddLevel(const CRpgEvent& com);
	void comOperateAddStatus(const CRpgEvent& com);
	void comOperateAddSkill(const CRpgEvent& com);
	void comOperateCallEvent(const CRpgEvent& com);
	void comOperateRoute(const CRpgEvent& com);
	void comOperateRouteStart(const CRpgEvent& com);
	void comOperateRouteEnd(const CRpgEvent& com);
	void comOperateNameInput(const CRpgEvent& com);
	void comOperatePlayerNameChange(const CRpgEvent& com);
	void comOperatePlayerTitleChange(const CRpgEvent& com);
	void comOperatePlayerWalkChange(const CRpgEvent& com);
	void comOperatePlayerFaceChange(const CRpgEvent& com);
	void comOperateBgm(const CRpgEvent& com);

	void comWaitLocateMove(const CRpgEvent& com);
	void comWaitTextShow(const CRpgEvent& com);
	void comWaitBattleStart(const CRpgEvent& com);
	void comWaitSelectStart(const CRpgEvent& com);
	void comWaitWait(const CRpgEvent& com);
	void comWaitPictureMove(const CRpgEvent& com);
	void comWaitMapScroll(const CRpgEvent& com);
	void comWaitNameInput(const CRpgEvent& com);

private:
	GameField*					gameField_;
	GameMessageWindow*			gameMessageWindow_;
	GameSelectWindow*			selectWindow_;
	GameNameInputMenu*			nameInputMenu_;
	MessageWindowSetting		messageWindowSetting_;
	kuto::SimpleArray<PageInfo>	eventPageInfos_;
	ComFuncMap					comFuncMap_;
	ComFuncMap					comWaitFuncMap_;
	TimerInfo					timer_;
	kuto::Array<int, 100>		labels_;
	int							currentEventIndex_;
	const CRpgEventList*		currentEventPage_;
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
};

