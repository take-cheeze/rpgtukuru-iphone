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
	typedef rpg2k::structure::Array1D EventPage;
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

	void comOperateSwitch(const rpg2k::structure::Instruction& com);
	void comOperateVar(const rpg2k::structure::Instruction& com);
	void comOperateItem(const rpg2k::structure::Instruction& com);
	void comOperateMoney(const rpg2k::structure::Instruction& com);
	void comOperateTimer(const rpg2k::structure::Instruction& com);
	void comOperateJumpLabel(const rpg2k::structure::Instruction& com);
	void comOperatePartyChange(const rpg2k::structure::Instruction& com);
	void comOperateLocateMove(const rpg2k::structure::Instruction& com);
	void comOperateLocateSave(const rpg2k::structure::Instruction& com);
	void comOperateLocateLoad(const rpg2k::structure::Instruction& com);
	void comOperateTextShow(const rpg2k::structure::Instruction& com);
	void comOperateTextOption(const rpg2k::structure::Instruction& com);
	void comOperateTextFace(const rpg2k::structure::Instruction& com);
	void comOperateBattleStart(const rpg2k::structure::Instruction& com);
	void comOperateBattleWin(const rpg2k::structure::Instruction& com);
	void comOperateBattleEscape(const rpg2k::structure::Instruction& com);
	void comOperateBattleLose(const rpg2k::structure::Instruction& com);
	void comOperateBranchEnd(const rpg2k::structure::Instruction& com);
	void comOperateIfStart(const rpg2k::structure::Instruction& com);
	void comOperateIfElse(const rpg2k::structure::Instruction& com);
	void comOperateSelectStart(const rpg2k::structure::Instruction& com);
	void comOperateSelectCase(const rpg2k::structure::Instruction& com);
	void comOperateGameOver(const rpg2k::structure::Instruction& com);
	void comOperateReturnTitle(const rpg2k::structure::Instruction& com);
	void comOperateEventBreak(const rpg2k::structure::Instruction& com);
	void comOperateEventClear(const rpg2k::structure::Instruction& com);
	void comOperateLoopStart(const rpg2k::structure::Instruction& com);
	void comOperateLoopBreak(const rpg2k::structure::Instruction& com);
	void comOperateLoopEnd(const rpg2k::structure::Instruction& com);
	void comOperateWait(const rpg2k::structure::Instruction& com);
	void comOperatePictureShow(const rpg2k::structure::Instruction& com);
	void comOperatePictureMove(const rpg2k::structure::Instruction& com);
	void comOperatePictureClear(const rpg2k::structure::Instruction& com);
	void comOperateFadeType(const rpg2k::structure::Instruction& com);
	void comOperateFadeOut(const rpg2k::structure::Instruction& com);
	void comOperateFadeIn(const rpg2k::structure::Instruction& com);
	void comOperateMapScroll(const rpg2k::structure::Instruction& com);
	void comOperatePlayerVisible(const rpg2k::structure::Instruction& com);
	void comOperatePlayerCure(const rpg2k::structure::Instruction& com);
	void comOperateAddExp(const rpg2k::structure::Instruction& com);
	void comOperateAddLevel(const rpg2k::structure::Instruction& com);
	void comOperateAddStatus(const rpg2k::structure::Instruction& com);
	void comOperateAddSkill(const rpg2k::structure::Instruction& com);
	void comOperateCallEvent(const rpg2k::structure::Instruction& com);
	void comOperateRoute(const rpg2k::structure::Instruction& com);
	void comOperateRouteStart(const rpg2k::structure::Instruction& com);
	void comOperateRouteEnd(const rpg2k::structure::Instruction& com);
	void comOperateNameInput(const rpg2k::structure::Instruction& com);
	void comOperatePlayerNameChange(const rpg2k::structure::Instruction& com);
	void comOperatePlayerTitleChange(const rpg2k::structure::Instruction& com);
	void comOperatePlayerWalkChange(const rpg2k::structure::Instruction& com);
	void comOperatePlayerFaceChange(const rpg2k::structure::Instruction& com);
	void comOperateBgm(const rpg2k::structure::Instruction& com);
	void comOperateKey(const rpg2k::structure::Instruction& com);
	void comOperatePanorama(const rpg2k::structure::Instruction& com);
	void comOperateInnStart(const rpg2k::structure::Instruction& com);
	void comOperateInnOk(const rpg2k::structure::Instruction& com);
	void comOperateInnCancel(const rpg2k::structure::Instruction& com);
	void comOperateShopStart(const rpg2k::structure::Instruction& com);
	void comOperatePlaySound(const rpg2k::structure::Instruction& com);
	void comOperateScreenColor(const rpg2k::structure::Instruction& com);
	void comOperateBattleAnime(const rpg2k::structure::Instruction& com);
	void comOperateEquip(const rpg2k::structure::Instruction& com);

	void comWaitLocateMove(const rpg2k::structure::Instruction& com);
	void comWaitTextShow(const rpg2k::structure::Instruction& com);
	void comWaitBattleStart(const rpg2k::structure::Instruction& com);
	void comWaitSelectStart(const rpg2k::structure::Instruction& com);
	void comWaitWait(const rpg2k::structure::Instruction& com);
	void comWaitPictureMove(const rpg2k::structure::Instruction& com);
	void comWaitMapScroll(const rpg2k::structure::Instruction& com);
	void comWaitNameInput(const rpg2k::structure::Instruction& com);
	void comWaitKey(const rpg2k::structure::Instruction& com);
	void comWaitInnStart(const rpg2k::structure::Instruction& com);
	void comWaitShopStart(const rpg2k::structure::Instruction& com);
	void comWaitScreenColor(const rpg2k::structure::Instruction& com);
	void comWaitBattleAnime(const rpg2k::structure::Instruction& com);

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

