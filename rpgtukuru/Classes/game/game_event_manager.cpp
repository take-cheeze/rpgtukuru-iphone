/**
 * @file
 * @brief Game Event Manager
 * @author project.kuto
 */

#include <utility>
#include <kuto/kuto_utility.h>
#include <kuto/kuto_error.h>
#include <kuto/kuto_virtual_pad.h>
#include "game_event_manager.h"
#include "game_field.h"
#include "game_map.h"
#include "game_player.h"
#include "game_npc.h"
#include "game_message_window.h"
#include "game_collision.h"
#include "game_inventory.h"
#include "game_battle.h"
#include "game_select_window.h"
#include "game_event_picture.h"
#include "game_name_input_menu.h"
#include "game_bgm.h"


GameEventManager::GameEventManager(kuto::Task* parent, GameField* field)
: kuto::Task(parent)
, gameField_(field)
, currentEventPage_(NULL), executeChildCommands_(true), encountStep_(0), routeSetChara_(NULL)
{
	bgm_ = GameBgm::createTask(this, "");	// temp
	// const CRpgLmu& rpgLmu = gameField_->getMap()->getRpgLmu();
	initEventPageInfos();
	updateEventAppear();
	gameMessageWindow_ = GameMessageWindow::createTask(this, gameField_->getGameSystem());
	gameMessageWindow_->setPosition(kuto::Vector2(0.f, 160.f));
	gameMessageWindow_->setSize(kuto::Vector2(320.f, 80.f));
	gameMessageWindow_->setMessageAlign(GameMessageWindow::kAlignLeft);
	gameMessageWindow_->pauseUpdate(true);
	selectWindow_ = GameSelectWindow::createTask(this, gameField_->getGameSystem());
	selectWindow_->setPosition(kuto::Vector2(0.f, 160.f));
	selectWindow_->setSize(kuto::Vector2(320.f, 80.f));
	selectWindow_->setMessageAlign(GameMessageWindow::kAlignLeft);
	selectWindow_->pauseUpdate(true);
	nameInputMenu_ = GameNameInputMenu::createTask(this, gameField_->getGameSystem());
	nameInputMenu_->pauseUpdate(true);
	
	comFuncMap_[CODE_OPERATE_SWITCH] = &GameEventManager::comOperateSwitch;
	comFuncMap_[CODE_OPERATE_VAR] = &GameEventManager::comOperateVar;
	comFuncMap_[CODE_OPERATE_ITEM] = &GameEventManager::comOperateItem;
	comFuncMap_[CODE_OPERATE_MONEY] = &GameEventManager::comOperateMoney;
	comFuncMap_[CODE_OPERATE_TIMER] = &GameEventManager::comOperateTimer;
	comFuncMap_[CODE_GOTO_LABEL] = &GameEventManager::comOperateJumpLabel;
	comFuncMap_[CODE_PARTY_CHANGE] = &GameEventManager::comOperatePartyChange;
	comFuncMap_[CODE_LOCATE_MOVE] = &GameEventManager::comOperateLocateMove;
	comFuncMap_[CODE_LOCATE_SAVE] = &GameEventManager::comOperateLocateSave;
	comFuncMap_[CODE_LOCATE_LOAD] = &GameEventManager::comOperateLocateLoad;
	comFuncMap_[CODE_TXT_SHOW] = &GameEventManager::comOperateTextShow;
	comFuncMap_[CODE_TXT_OPTION] = &GameEventManager::comOperateTextOption;
	comFuncMap_[CODE_TXT_FACE] = &GameEventManager::comOperateTextFace;
	comFuncMap_[CODE_BTL_GO_START] = &GameEventManager::comOperateBattleStart;
	comFuncMap_[CODE_BTL_GO_WIN] = &GameEventManager::comOperateBattleWin;
	comFuncMap_[CODE_BTL_GO_ESCAPE] = &GameEventManager::comOperateBattleEscape;
	comFuncMap_[CODE_BTL_GO_LOSE] = &GameEventManager::comOperateBattleLose;
	comFuncMap_[CODE_BTL_GO_END] = &GameEventManager::comOperateBranchEnd;
	comFuncMap_[CODE_IF_START] = &GameEventManager::comOperateIfStart;
	comFuncMap_[CODE_IF_ELSE] = &GameEventManager::comOperateIfElse;
	comFuncMap_[CODE_IF_END] = &GameEventManager::comOperateBranchEnd;
	comFuncMap_[CODE_SELECT_START] = &GameEventManager::comOperateSelectStart;
	comFuncMap_[CODE_SELECT_CASE] = &GameEventManager::comOperateSelectCase;
	comFuncMap_[CODE_SELECT_END] = &GameEventManager::comOperateBranchEnd;
	comFuncMap_[CODE_GAMEOVER] = &GameEventManager::comOperateGameOver;
	comFuncMap_[CODE_TITLE] = &GameEventManager::comOperateReturnTitle;
	comFuncMap_[CODE_EVENT_BREAK] = &GameEventManager::comOperateEventBreak;
	comFuncMap_[CODE_EVENT_CLEAR] = &GameEventManager::comOperateEventClear;
	comFuncMap_[CODE_LOOP_START] = &GameEventManager::comOperateLoopStart;
	comFuncMap_[CODE_LOOP_BREAK] = &GameEventManager::comOperateLoopBreak;
	comFuncMap_[CODE_LOOP_END] = &GameEventManager::comOperateLoopEnd;
	comFuncMap_[CODE_WAIT] = &GameEventManager::comOperateWait;
	comFuncMap_[CODE_PICT_SHOW] = &GameEventManager::comOperatePictureShow;
	comFuncMap_[CODE_PICT_MOVE] = &GameEventManager::comOperatePictureMove;
	comFuncMap_[CODE_PICT_CLEAR] = &GameEventManager::comOperatePictureClear;
	comFuncMap_[CODE_SYSTEM_SCREEN] = &GameEventManager::comOperateFadeType;
	comFuncMap_[CODE_SCREEN_CLEAR] = &GameEventManager::comOperateFadeOut;
	comFuncMap_[CODE_SCREEN_SHOW] = &GameEventManager::comOperateFadeIn;
	comFuncMap_[CODE_SCREEN_SCROLL] = &GameEventManager::comOperateMapScroll;
	comFuncMap_[CODE_CHARA_TRANS] = &GameEventManager::comOperatePlayerVisible;
	comFuncMap_[CODE_PARTY_REFRESH] = &GameEventManager::comOperatePlayerCure;
	comFuncMap_[CODE_PARTY_EXP] = &GameEventManager::comOperateAddExp;
	comFuncMap_[CODE_PARTY_LV] = &GameEventManager::comOperateAddLevel;
	comFuncMap_[CODE_PARTY_POWER] = &GameEventManager::comOperateAddStatus;
	comFuncMap_[CODE_PARTY_SKILL] = &GameEventManager::comOperateAddSkill;
	comFuncMap_[CODE_EVENT_GOSUB] = &GameEventManager::comOperateCallEvent;
	comFuncMap_[CODE_CHARA_MOVE] = &GameEventManager::comOperateRoute;
	comFuncMap_[CODE_MOVEALL_START] = &GameEventManager::comOperateRouteStart;
	comFuncMap_[CODE_MOVEALL_CANSEL] = &GameEventManager::comOperateRouteEnd;
	comFuncMap_[CODE_NAME_INPUT] = &GameEventManager::comOperateNameInput;
	comFuncMap_[CODE_PARTY_NAME] = &GameEventManager::comOperatePlayerNameChange;
	comFuncMap_[CODE_PARTY_TITLE] = &GameEventManager::comOperatePlayerTitleChange;
	comFuncMap_[CODE_PARTY_WALK] = &GameEventManager::comOperatePlayerWalkChange;
	comFuncMap_[CODE_PARTY_FACE] = &GameEventManager::comOperatePlayerFaceChange;
	comFuncMap_[CODE_SYSTEM_BGM] = &GameEventManager::comOperateBgm;
	comFuncMap_[CODE_OPERATE_KEY] = &GameEventManager::comOperateKey;
	comFuncMap_[CODE_PANORAMA] = &GameEventManager::comOperatePanorama;

	comWaitFuncMap_[CODE_LOCATE_MOVE] = &GameEventManager::comWaitLocateMove;	
	comWaitFuncMap_[CODE_LOCATE_LOAD] = &GameEventManager::comWaitLocateMove;	
	comWaitFuncMap_[CODE_TXT_SHOW] = &GameEventManager::comWaitTextShow;	
	comWaitFuncMap_[CODE_BTL_GO_START] = &GameEventManager::comWaitBattleStart;	
	comWaitFuncMap_[CODE_SELECT_START] = &GameEventManager::comWaitSelectStart;	
	comWaitFuncMap_[CODE_WAIT] = &GameEventManager::comWaitWait;
	comWaitFuncMap_[CODE_PICT_MOVE] = &GameEventManager::comWaitPictureMove;
	comWaitFuncMap_[CODE_SCREEN_SCROLL] = &GameEventManager::comWaitMapScroll;
	comWaitFuncMap_[CODE_PARTY_EXP] = &GameEventManager::comWaitTextShow;	
	comWaitFuncMap_[CODE_PARTY_LV] = &GameEventManager::comWaitTextShow;	
	comWaitFuncMap_[CODE_NAME_INPUT] = &GameEventManager::comWaitNameInput;	
	comWaitFuncMap_[CODE_OPERATE_KEY] = &GameEventManager::comWaitKey;	
	
	pictures_.zeromemory();	
}

bool GameEventManager::initialize()
{
	if (isInitializedChildren()) {
		gameMessageWindow_->pauseUpdate(false);
		gameMessageWindow_->freeze(true);
		selectWindow_->pauseUpdate(false);
		selectWindow_->freeze(true);
		nameInputMenu_->pauseUpdate(false);
		nameInputMenu_->freeze(true);
		
		bgm_->play();		// temp
		return true;
	}
	return false;
}

void GameEventManager::preMapChange()
{
	for (u32 i = 0; i < pictures_.size(); i++) {
		if (pictures_[i]) {
			pictures_[i]->release();
			pictures_[i] = NULL;
		}
	}
	const CRpgLmu& rpgLmu = gameField_->getMap()->getRpgLmu();
	for (u32 i = 1; i < rpgLmu.saMapEvent.GetSize(); i++) {
		if (eventPageInfos_[i].npc) {
			eventPageInfos_[i].npc->release();
			eventPageInfos_[i].npc = NULL;
		}
	}
		
	restEventInfo_.enable = true;
	restEventInfo_.eventIndex = waitEventInfo_.eventIndex;
	restEventInfo_.eventListCopy = *waitEventInfo_.page;	// deep copyしとく
	restEventInfo_.pos = waitEventInfo_.pos;
	restEventInfo_.nextPos = waitEventInfo_.nextPos;
	restEventInfo_.count = waitEventInfo_.count;
	restEventInfo_.executeChildCommands = waitEventInfo_.executeChildCommands;
	restEventInfo_.conditionStack = waitEventInfo_.conditionStack;
	restEventInfo_.loopStack = waitEventInfo_.loopStack;
	
	waitEventInfo_.page = &restEventInfo_.eventListCopy;	// pointerを差し替え
	callStack_.clear();		// これも消滅ってことで
}

void GameEventManager::initEventPageInfos()
{
	const CRpgLdb& rpgLdb = gameField_->getGameSystem().getRpgLdb();
	const CRpgLmu& rpgLmu = gameField_->getMap()->getRpgLmu();
	eventPageInfos_.deallocate();
	eventPageInfos_.allocate(rpgLmu.saMapEvent.GetSize() + rpgLdb.saCommonEvent.GetSize());
}

void GameEventManager::postMapChange()
{
	initEventPageInfos();
	updateEventAppear();
}

void GameEventManager::update()
{
	if (timer_.enable) {
		if (timer_.count > 0)
			timer_.count--;
	}
	updateEventAppear();
	if (waitEventInfo_.enable)
		updateWaitEvent();
	else
		updateEvent();
	if (!waitEventInfo_.enable) {
		kuto::VirtualPad* virtualPad = kuto::VirtualPad::instance();
		bool pressMenu = virtualPad->press(kuto::VirtualPad::KEY_B);
		pressMenu = pressMenu && !gameField_->getPlayerLeader()->isMoving();
		// Undefined cannot open menu flag
		if (pressMenu) {
			gameField_->startSystemMenu();
		} else
			updateEncount();
	}
}

std::string GameEventManager::getEncountBattleMap(const CRpgLmt::MapInfo& mapInfo, int terrainId)
{
	if (mapInfo.m_BattleMapType == 0) {
		const CRpgLmt::MapInfo& parentInfo = gameField_->getGameSystem().getRpgLmt().m_saMapInfo[mapInfo.m_ParentMapID];
		return getEncountBattleMap(parentInfo, terrainId);
	} else if (mapInfo.m_BattleMapType == 1) {
		const CRpgLdb& ldb = gameField_->getGameSystem().getRpgLdb();
		return ldb.saTerrain[terrainId].battleGraphic;
	} else {
		return mapInfo.m_BattleMapName;
	}
}

void GameEventManager::updateEncount()
{
	if (gameField_->getGameSystem().getConfig().noEncount)
		return;
	
	GamePlayer* player = gameField_->getPlayerLeader();
	if (player->getMoveResult() == GameChara::kMoveResultDone) {
		encountStep_++;
		const CRpgLdb& ldb = gameField_->getGameSystem().getRpgLdb();
		const CRpgLmt::MapInfo& mapInfo = gameField_->getGameSystem().getRpgLmt().m_saMapInfo[gameField_->getMap()->getMapId()];
		float encountRatio = mapInfo.m_EnemyEncounter > 0? (float)(encountStep_) / ((float)mapInfo.m_EnemyEncounter * 2.f + 1.f) : -1.f;
		if (encountStep_ > 0 && encountRatio >= kuto::random(1.f) && mapInfo.m_saEnemyGroup.GetSize() > 0) {
			int terrainId = gameField_->getMap()->getTerrainId(player->getPosition().x, player->getPosition().y);
			std::string terrain = getEncountBattleMap(mapInfo, terrainId);
			std::vector<int> enableEnemyIds;
			for (u32 i = 0; i < mapInfo.m_saEnemyGroup.GetSize(); i++) {
				int enemyGroupId = mapInfo.m_saEnemyGroup[i].enemyGroupID;
				if (mapInfo.m_BattleMapType != 2 ||
				 (terrainId > 0 && ((int)ldb.saEnemyGroup[enemyGroupId].appearTerrain.size() <= terrainId ||
				 ldb.saEnemyGroup[enemyGroupId].appearTerrain[terrainId]))) {
					enableEnemyIds.push_back(enemyGroupId);
				}
			}
			if (!enableEnemyIds.empty()) {
				gameField_->startBattle(terrain, enableEnemyIds[kuto::random((int)enableEnemyIds.size())], false, true, true);
				encountStep_ = -1;
			}
		}
	}
}

bool GameEventManager::isEventConditionOk(const CRpgEventCondition& condition)
{
	GameSystem& system = gameField_->getGameSystem();
	bool appear = true;
	if (condition.nFlag & CRpgEventCondition::kFlagSwitch1) {
		if (!system.getSwitch(condition.nSw1))
			appear = false;
	}
	if (condition.nFlag & CRpgEventCondition::kFlagSwitch2) {
		if (!system.getSwitch(condition.nSw2))
			appear = false;
	}
	if (condition.nFlag & CRpgEventCondition::kFlagVar) {
		if (system.getVar(condition.nVarNum) < condition.nVarOver)
			appear = false;
	}
	if (condition.nFlag & CRpgEventCondition::kFlagItem) {
		if (system.getInventory()->getItemNum(condition.nItem) == 0)
			appear = false;
	}
	if (condition.nFlag & CRpgEventCondition::kFlagPlayer) {
		if (!gameField_->getPlayerFromId(condition.nChara))
			appear = false;
	}
	if (condition.nFlag & CRpgEventCondition::kFlagTimer) {
		if (!timer_.enable || ((timer_.count + 59) / 60) > condition.nTimer)
			appear = false;
	}
	return appear;
}

void GameEventManager::updateEventAppear()
{
	const CRpgLmu& rpgLmu = gameField_->getMap()->getRpgLmu();
	GameSystem& system = gameField_->getGameSystem();
	for (u32 i = 1; i < rpgLmu.saMapEvent.GetSize(); i++) {
		const CRpgLmu::MAPEVENT& mapEvent = rpgLmu.saMapEvent[i];
		if (eventPageInfos_[i].cleared)
			continue;
		int pageIndex = 0;
		for (int iPage = mapEvent.saPage.GetSize() - 1; iPage >= 1; iPage--) {
			const EventPage& eventPage = mapEvent.saPage[iPage];
			bool appear = isEventConditionOk(eventPage.eventList.condition);
			if (appear) {
				pageIndex = iPage;
				break;
			}
		}
		// change page
		if (eventPageInfos_[i].index != pageIndex) {
			if (pageIndex > 0) {
				const EventPage& eventPage = mapEvent.saPage[pageIndex];
				u32 npcCrc = kuto::crc32(eventPage.strWalk);
				if (eventPageInfos_[i].npc != NULL && eventPageInfos_[i].npcCrc != npcCrc) {
					gameField_->getCollision()->removeChara(eventPageInfos_[i].npc);
					eventPageInfos_[i].npc->release();
					eventPageInfos_[i].npc = NULL;
				}
				if (!eventPage.strWalk.empty()) {
					if (eventPageInfos_[i].npc) {
						eventPageInfos_[i].npc->setEventPage(eventPage);
						eventPageInfos_[i].npc->setDirection((GameChara::DirType)eventPage.nWalkMuki);
					} else {
						GameNpc* npc = GameNpc::createTask(this, gameField_, eventPage);
						npc->setPosition(GameChara::Point(mapEvent.x, mapEvent.y));
						npc->loadWalkTexture(eventPage.strWalk, eventPage.nWalkPos);
						npc->setDirection((GameChara::DirType)eventPage.nWalkMuki);
						
						gameField_->getCollision()->addChara(npc);
						eventPageInfos_[i].npc = npc;
					}
				}
				eventPageInfos_[i].npcCrc = npcCrc;
			} else {
				if (eventPageInfos_[i].npc != NULL) {
					gameField_->getCollision()->removeChara(eventPageInfos_[i].npc);
					eventPageInfos_[i].npc->release();
					eventPageInfos_[i].npc = NULL;
				}
				eventPageInfos_[i].npcCrc = 0;
			}
			eventPageInfos_[i].index = pageIndex;
		}
		// update position
		if (eventPageInfos_[i].index > 0) {
			if (eventPageInfos_[i].npc != NULL) {
				eventPageInfos_[i].x = eventPageInfos_[i].npc->getPosition().x;
				eventPageInfos_[i].y = eventPageInfos_[i].npc->getPosition().y;
			} else {
				eventPageInfos_[i].x = mapEvent.x;
				eventPageInfos_[i].y = mapEvent.y;
			}
		}
	}

	for (u32 i = 1; i < system.getRpgLdb().saCommonEvent.GetSize(); i++) {
		int pageInfoIndex = i + rpgLmu.saMapEvent.GetSize();
		const CRpgLdb::CommonEvent& commonEvent = system.getRpgLdb().saCommonEvent[i];
		int pageIndex = 0;
		bool appear = isEventConditionOk(commonEvent.eventList.condition);
		if (appear) {
			pageIndex = 1;
		}
		eventPageInfos_[pageInfoIndex].index = pageIndex;
	}
}

void GameEventManager::updateEvent()
{
	const CRpgLmu& rpgLmu = gameField_->getMap()->getRpgLmu();
	GameSystem& system = gameField_->getGameSystem();

	kuto::VirtualPad* virtualPad = kuto::VirtualPad::instance();
	bool pressOk = virtualPad->press(kuto::VirtualPad::KEY_A);
	GamePlayer* player = gameField_->getPlayerLeader();
	const GameChara::Point& playerPos = player->getPosition();
	GameChara::DirType playerDir = player->getDirection();

	
	for (u32 i = 1; i < rpgLmu.saMapEvent.GetSize(); i++) {
		const CRpgLmu::MAPEVENT& mapEvent = rpgLmu.saMapEvent[i];
		if (eventPageInfos_[i].cleared)
			continue;
		currentEventIndex_ = i;
		if (eventPageInfos_[i].index > 0) {
			const EventPage& eventPage = mapEvent.saPage[eventPageInfos_[i].index];
			bool isStart = false;
			switch (eventPage.eventList.condition.nStart) {
			case CRpgEventCondition::kStartTypeButton:
				if (!waitEventInfo_.enable && pressOk) {
					if (eventPage.priority == CRpgLmu::kPriorityNormal) {
						isStart = 
							(playerPos.x - eventPageInfos_[i].x == 1 && playerPos.y - eventPageInfos_[i].y == 0 && playerDir == GameChara::kDirLeft)
						||  (playerPos.x - eventPageInfos_[i].x == -1 && playerPos.y - eventPageInfos_[i].y == 0 && playerDir == GameChara::kDirRight)
						||  (playerPos.x - eventPageInfos_[i].x == 0 && playerPos.y - eventPageInfos_[i].y == 1 && playerDir == GameChara::kDirUp)
						||  (playerPos.x - eventPageInfos_[i].x == 0 && playerPos.y - eventPageInfos_[i].y == -1 && playerDir == GameChara::kDirDown);
					} else {
						isStart = playerPos.x - eventPageInfos_[i].x == 0 && playerPos.y - eventPageInfos_[i].y == 0;
					}
				}
				break;
			case CRpgEventCondition::kStartTypeTouchPlayer:
				if (!waitEventInfo_.enable) {
					if (eventPage.priority == CRpgLmu::kPriorityNormal) {
						isStart = 
							(playerPos.x - eventPageInfos_[i].x == 1 && playerPos.y - eventPageInfos_[i].y == 0 && playerDir == GameChara::kDirLeft)
						||  (playerPos.x - eventPageInfos_[i].x == -1 && playerPos.y - eventPageInfos_[i].y == 0 && playerDir == GameChara::kDirRight)
						||  (playerPos.x - eventPageInfos_[i].x == 0 && playerPos.y - eventPageInfos_[i].y == 1 && playerDir == GameChara::kDirUp)
						||  (playerPos.x - eventPageInfos_[i].x == 0 && playerPos.y - eventPageInfos_[i].y == -1 && playerDir == GameChara::kDirDown);
						isStart = isStart && player->getMoveResult() == GameChara::kMoveResultCollied;
					} else {
						isStart = playerPos.x - eventPageInfos_[i].x == 0 && playerPos.y - eventPageInfos_[i].y == 0;
						isStart = isStart && player->getMoveResult() == GameChara::kMoveResultDone;
						isStart = isStart && !player->isEnableRoute();
					}
				}
				break;
			case CRpgEventCondition::kStartTypeTouchEvent:
				if (!waitEventInfo_.enable && eventPageInfos_[i].npc) {
					if (eventPage.priority == CRpgLmu::kPriorityNormal) {
						GameChara::DirType npcDir = eventPageInfos_[i].npc->getDirection();
						isStart = 
							(playerPos.x - eventPageInfos_[i].x == 1 && playerPos.y - eventPageInfos_[i].y == 0 && npcDir == GameChara::kDirRight)
						||  (playerPos.x - eventPageInfos_[i].x == -1 && playerPos.y - eventPageInfos_[i].y == 0 && npcDir == GameChara::kDirLeft)
						||  (playerPos.x - eventPageInfos_[i].x == 0 && playerPos.y - eventPageInfos_[i].y == 1 && npcDir == GameChara::kDirDown)
						||  (playerPos.x - eventPageInfos_[i].x == 0 && playerPos.y - eventPageInfos_[i].y == -1 && npcDir == GameChara::kDirUp);
						isStart = isStart && eventPageInfos_[i].npc->getMoveResult() == GameChara::kMoveResultCollied;
					} else {
						isStart = playerPos.x - eventPageInfos_[i].x == 0 && playerPos.y - eventPageInfos_[i].y == 0;
						isStart = isStart && eventPageInfos_[i].npc->getMoveResult() == GameChara::kMoveResultDone;
						
					}
				}
				break;
			case CRpgEventCondition::kStartTypeAuto:
				if (!waitEventInfo_.enable) {
					isStart = true;
				}
				break;
			case CRpgEventCondition::kStartTypeParallel:
				//if (!nextWaitEventInfo_.enable)
					isStart = true;
				break;
			}
			if (!isStart)
				continue;
			
			startDecideButton_ = eventPage.eventList.condition.nStart == CRpgEventCondition::kStartTypeButton;
			executeChildCommands_ = true;
			conditionStack_.clear();
			loopStack_.clear();
			routeSetChara_ = NULL;
			backupWaitInfoEnable_ = waitEventInfo_.enable;
			waitEventInfo_.enable = false;
			executeCommands(eventPage.eventList, 0);
			if (backupWaitInfoEnable_)
				waitEventInfo_.enable = true;
		}
	}
	for (u32 i = 1; i < system.getRpgLdb().saCommonEvent.GetSize(); i++) {
		currentEventIndex_ = i + rpgLmu.saMapEvent.GetSize();
		if (eventPageInfos_[currentEventIndex_].index > 0) {
			const CRpgLdb::CommonEvent& eventPage = system.getRpgLdb().saCommonEvent[i];
			bool isStart = false;
			switch (eventPage.eventList.condition.nStart) {
			case CRpgEventCondition::kStartTypeAuto:
				if (!waitEventInfo_.enable) {
					isStart = true;
				}
				break;
			case CRpgEventCondition::kStartTypeParallel:
				//if (!nextWaitEventInfo_.enable)
					isStart = true;
				break;
			}
			if (!isStart)
				continue;
			
			startDecideButton_ = eventPage.eventList.condition.nStart == CRpgEventCondition::kStartTypeButton;
			executeChildCommands_ = true;
			conditionStack_.clear();
			loopStack_.clear();
			routeSetChara_ = NULL;
			backupWaitInfoEnable_ = waitEventInfo_.enable;
			waitEventInfo_.enable = false;
			executeCommands(eventPage.eventList, 0);
			if (backupWaitInfoEnable_)
				waitEventInfo_.enable = true;
		}
	}
}

void GameEventManager::executeCommands(const CRpgEventList& eventPage, int start)
{
	currentEventPage_ = &eventPage;
	// create label
	std::memset(labels_.get(), 0, labels_.size() * sizeof(int));
	for (currentCommandIndex_ = 0; currentCommandIndex_ < (int)eventPage.events.size(); currentCommandIndex_++) {
		const CRpgEvent& com = 	eventPage.events[currentCommandIndex_];
		if (com.getEventCode() == CODE_LABEL) {
			labels_[com.getIntParam(0) - 1] = currentCommandIndex_;
		}
	}
	// execute command
	for (currentCommandIndex_ = start; currentCommandIndex_ < (int)eventPage.events.size(); currentCommandIndex_++) {
		const CRpgEvent& com = eventPage.events[currentCommandIndex_];
		if (!conditionStack_.empty() && conditionStack_.top().nest < com.getNest() && !executeChildCommands_) {
			continue;
		}
		//executeChildCommands_ = false;
		
		ComFuncMap::iterator it = comFuncMap_.find(com.getEventCode());
		if (it != comFuncMap_.end()) {
			(this->*(it->second))(com);
			if (waitEventInfo_.enable) {
				if (backupWaitInfoEnable_) {
					// need to push to call stack??
				} else {
					waitEventInfo_.count = 0;
					waitEventInfo_.eventIndex = currentEventIndex_;
					waitEventInfo_.page = &eventPage;
					waitEventInfo_.pos = currentCommandIndex_;
					waitEventInfo_.nextPos = waitEventInfo_.pos + 1;
					waitEventInfo_.conditionStack = conditionStack_;
					waitEventInfo_.loopStack = loopStack_;
					waitEventInfo_.executeChildCommands = executeChildCommands_;
					
					GamePlayer* player = gameField_->getPlayerLeader();
					player->startTalking(player->getDirection());
					if (&restEventInfo_.eventListCopy != &eventPage && eventPageInfos_[currentEventIndex_].npc) {
						const GameChara::Point& playerPos = player->getPosition();
						const GameChara::Point& npcPos = eventPageInfos_[currentEventIndex_].npc->getPosition();
						GameChara::DirType dir = GameChara::kDirLeft;
						if (playerPos.x > npcPos.x)
							dir = GameChara::kDirRight;
						else if (playerPos.y > npcPos.y)
							dir = GameChara::kDirDown;
						else if (playerPos.y < npcPos.y)
							dir = GameChara::kDirUp;
						eventPageInfos_[currentEventIndex_].npc->startTalking(dir);
					}
				}
				break;
			}
		} else {
			//kuto_printf("unknown command %x¥n", com.getEventCode());
		}
		//kuto_printf("event command %x¥n", com.getEventCode());
	}
	if (!waitEventInfo_.enable && !backupWaitInfoEnable_) {
		gameMessageWindow_->setFaceTexture("", 0, false, false);	// reset face?
	}
}

void GameEventManager::updateWaitEvent()
{
	const CRpgEventList& eventPage = *waitEventInfo_.page;
	int comStartPos = waitEventInfo_.pos;
	executeChildCommands_ = waitEventInfo_.executeChildCommands;
	conditionStack_ = waitEventInfo_.conditionStack;
	loopStack_ = waitEventInfo_.loopStack;
	routeSetChara_ = NULL;
	// execute command
	const CRpgEvent& com = eventPage.events[comStartPos];
	ComFuncMap::iterator it = comWaitFuncMap_.find(com.getEventCode());
	kuto_assert(it != comWaitFuncMap_.end());
	(this->*(it->second))(com);
	
	if (!waitEventInfo_.enable) {
		gameField_->getPlayerLeader()->endTalking();
		if (&restEventInfo_.eventListCopy != &eventPage && eventPageInfos_[waitEventInfo_.eventIndex].npc)
			eventPageInfos_[waitEventInfo_.eventIndex].npc->endTalking();
		
		restEventInfo_.enable = false;
		currentEventIndex_ = waitEventInfo_.eventIndex;
		backupWaitInfoEnable_ = false;
		
		executeCommands(eventPage, waitEventInfo_.nextPos);
		while (!waitEventInfo_.enable && !callStack_.empty()) {
			restoreCallStack();
			executeCommands(*currentEventPage_, currentCommandIndex_ + 1);
		}
	}
}

GameChara* GameEventManager::getCharaFromEventId(int eventId)
{
	GameChara* chara = NULL;
	switch (eventId) {
	case 10001:		// 主人公
		chara = gameField_->getPlayerLeader();
		break;
	case 10002:		// 小型船 Undefined
		chara = gameField_->getPlayerLeader();
		break;
	case 10003:		// 大型船 Undefined
		chara = gameField_->getPlayerLeader();
		break;
	case 10004:		// 飛行船 Undefined
		chara = gameField_->getPlayerLeader();
		break;
	case 10005:		// このイベント
		chara = eventPageInfos_[currentEventIndex_].npc;
		break;
	default:		// 呼び出しているマップ中のこのIDをもつマップイベント
		chara = eventPageInfos_[eventId].npc;
		break;
	}
	return chara;
}

void GameEventManager::comOperateSwitch(const CRpgEvent& com)
{
	GameSystem& system = gameField_->getGameSystem();
	int rangeA = 0;
	int rangeB = 0;
	switch (com.getIntParam(0)) {
	case 0:		// 0:[単独] S[A]
		rangeA = com.getIntParam(1);
		rangeB = rangeA;
		break;
	case 1:		// 1:[一括] S[A]〜S[B]
		rangeA = com.getIntParam(1);
		rangeB = com.getIntParam(2);
		break;
	case 2:		// 2:[変数] S[V[A]]
		rangeA = system.getVar(com.getIntParam(1));
		rangeB = rangeA;
		break;
	}
	for (int iSwitch = rangeA; iSwitch <= rangeB; iSwitch++) {
		if (com.getIntParam(3) == 0) {			// 0:ONにする
			system.setSwitch(iSwitch, true);
		} else if (com.getIntParam(3) == 1) {	// 1:OFFにする
			system.setSwitch(iSwitch, false);
		} else if (com.getIntParam(3) == 2) {	// 2:ON/OFFを逆転
			system.setSwitch(iSwitch, !system.getSwitch(iSwitch));
		}
	}	
}

void GameEventManager::comOperateVar(const CRpgEvent& com)
{
	GameSystem& system = gameField_->getGameSystem();
	int rangeA = 0;
	int rangeB = 0;
	switch (com.getIntParam(0)) {
	case 0:		// 0:[単独] S[A]
		rangeA = com.getIntParam(1);
		rangeB = rangeA;
		break;
	case 1:		// 1:[一括] S[A]〜S[B]
		rangeA = com.getIntParam(1);
		rangeB = com.getIntParam(2);
		break;
	case 2:		// 2:[変数] S[V[A]]
		rangeA = system.getVar(com.getIntParam(1));
		rangeB = rangeA;
		break;
	}
	int value = 0;
	switch (com.getIntParam(4)) {
	case 0:		// [定数] Cの値を直接適用
		value = com.getIntParam(5);
		break;
	case 1:		// [変数] V[C]
		value = system.getVar(com.getIntParam(5));
		break;
	case 2:		// [変数(参照)] V[V[C]]
		value = system.getVar(system.getVar(com.getIntParam(5)));
		break;
	case 3:		// [乱数] C〜Dの範囲の乱数
		value = kuto::random(com.getIntParam(6) + 1) + com.getIntParam(5);
		break;
	case 4:		// [アイテム]
		if (com.getIntParam(6) == 0)	// 所持数
			value = system.getInventory()->getItemNum(com.getIntParam(5));
		else {							// 装備数
			value = 0;
			for (u32 i = 0; i < gameField_->getPlayers().size(); i++) {
				const GameCharaStatus& status = gameField_->getPlayers()[i]->getStatus();
				if (status.getEquip().weapon == com.getIntParam(5) ||
				status.getEquip().shield == com.getIntParam(5) ||
				status.getEquip().protector == com.getIntParam(5) ||
				status.getEquip().helmet == com.getIntParam(5) ||
				status.getEquip().accessory == com.getIntParam(5))
					value++;
			}
		}
		break;
	case 5:		// [主人公]
		{
			const GameCharaStatus& status = gameField_->getGameSystem().getPlayerStatus(com.getIntParam(5));
			switch (com.getIntParam(6)) {
			case 0:		value = status.getLevel();		break;		// レベル
			case 1:		value = status.getExp();		break;		// 経験値
			case 2:		value = status.getHp();		break;		// HP
			case 3:		value = status.getMp();		break;		// MP
			case 4:		value = status.getBaseStatus().maxHP;		break;		// 最大HP
			case 5:		value = status.getBaseStatus().maxMP;		break;		// 最大MP
			case 6:		value = status.getBaseStatus().attack;		break;		// 攻撃力
			case 7:		value = status.getBaseStatus().defence;		break;		// 防御力
			case 8:		value = status.getBaseStatus().magic;		break;		// 精神力
			case 9:		value = status.getBaseStatus().speed;		break;		// 敏捷力
			case 10:	value = status.getEquip().weapon;		break;		// 武器No.
			case 11:	value = status.getEquip().shield;		break;		// 盾No.
			case 12:	value = status.getEquip().protector;		break;		// 鎧No.
			case 13:	value = status.getEquip().helmet;		break;		// 兜No.
			case 14:	value = status.getEquip().accessory;		break;		// 装飾品No.
			}
		}
		break;
	case 6:		// [キャラ]
		{
			GameChara* chara = getCharaFromEventId(com.getIntParam(5));
			switch (com.getIntParam(6)) {
			case 0:		// マップID
				switch (com.getIntParam(5)) {
				case 10002:		// 小型船 Undefined
				case 10003:		// 大型船 Undefined
				case 10004:		// 飛行船 Undefined
					value = 0;
					break;
				case 10001:		// 主人公
				case 10005:		// このイベント
				default:		// 呼び出しているマップ中のこのIDをもつマップイベント
					value = gameField_->getMap()->getMapId();
					break;
				}
				break;
			case 1:		// 	X座標
				if (chara)
					value = chara->getPosition().x;
				else
					value = 0;
				break;
			case 2:		// 	Y座標
				if (chara)
					value = chara->getPosition().y;
				else
					value = 0;
				break;
			case 3:		// 	向き
				if (chara) {
					switch (chara->getDirection()) {
					case GameChara::kDirUp:		value = 8;	break;
					case GameChara::kDirRight:	value = 6;	break;
					case GameChara::kDirDown:	value = 2;	break;
					case GameChara::kDirLeft:	value = 4;	break;
					}
				} else
					value = 0;
				break;
			case 4:		// 	画面X(画面上の水平座標)
				if (chara)
					value = chara->getPosition().x - gameField_->getMap()->getStartX();
				else
					value = 0;
				break;
			case 5:		// 	画面Y(画面上の垂直座標)
				if (chara)
					value = chara->getPosition().y - gameField_->getMap()->getStartY();
				else
					value = 0;
				break;
			}
		}
		break;
	case 7:		// [その他]
		if (com.getIntParam(5) == 0)		// 所持金
			value = system.getInventory()->getMoney();
		else if (com.getIntParam(5) == 1)	// タイマー1の残り秒数
			value = (timer_.count + 59) / 60;
		else if (com.getIntParam(5) == 2)	// パーティ人数
			value = gameField_->getPlayers().size();
		else if (com.getIntParam(5) == 3)	// セーブ回数
			value = system.getSaveCount();
		else if (com.getIntParam(5) == 4)	// 戦闘回数
			value = system.getBattleCount();
		else if (com.getIntParam(5) == 5)	// 勝利回数
			value = system.getWinCount();
		else if (com.getIntParam(5) == 6)	// 敗北回数
			value = system.getLoseCount();
		else if (com.getIntParam(5) == 7)	// 逃走回数
			value = system.getEscapeCount();
		else if (com.getIntParam(5) == 8)	// MIDIの演奏位置(Tick)
			value = 0;		// Undefined
		break;
	case 8:		// [敵キャラ]
		// Undefined
		break;
	}
	for (int iSwitch = rangeA; iSwitch <= rangeB; iSwitch++) {
		if (com.getIntParam(3) == 0) {			// 0:代入
			system.setVar(iSwitch, value);
		} else if (com.getIntParam(3) == 1) {	// 1:加算
			system.setVar(iSwitch, system.getVar(iSwitch) + value);
		} else if (com.getIntParam(3) == 2) {	// 2:減算
			system.setVar(iSwitch, system.getVar(iSwitch) - value);
		} else if (com.getIntParam(3) == 3) {	// 3:乗算
			system.setVar(iSwitch, system.getVar(iSwitch) * value);
		} else if (com.getIntParam(3) == 4) {	// 4:除算
			system.setVar(iSwitch, system.getVar(iSwitch) / value);
		} else if (com.getIntParam(3) == 5) {	// 5:剰余
			system.setVar(iSwitch, system.getVar(iSwitch) % value);
		}
	}	
}

void GameEventManager::comOperateItem(const CRpgEvent& com)
{
	GameSystem& system = gameField_->getGameSystem();
	int itemId = com.getIntParam(1) == 0? com.getIntParam(2) : system.getVar(com.getIntParam(2));
	int num = com.getIntParam(3) == 0? com.getIntParam(4) : system.getVar(com.getIntParam(4));
	system.getInventory()->addItemNum(itemId, com.getIntParam(0) == 0? num : -num);
}

void GameEventManager::comOperateMoney(const CRpgEvent& com)
{
	GameSystem& system = gameField_->getGameSystem();
	int num = com.getIntParam(1) == 0? com.getIntParam(2) : system.getVar(com.getIntParam(2));
	system.getInventory()->addMoney(com.getIntParam(0) == 0? num : -num);
}

void GameEventManager::comOperateTimer(const CRpgEvent& com)
{
	GameSystem& system = gameField_->getGameSystem();
	switch (com.getIntParam(0)) {
	case 0:		// 値の設定
		timer_.count = com.getIntParam(1) == 0? com.getIntParam(2) : system.getVar(com.getIntParam(2));
		timer_.count *= 60;		// sec to frame
		break;
	case 1:		// 作動開始
		timer_.enable = true;
		timer_.draw = (com.getIntParam(3) == 1);
		timer_.enableBattle = (com.getIntParam(4) == 1);
		break;
	case 2:		// 作動停止
		timer_.enable = false;
		break;	
	}
}

void GameEventManager::comOperateJumpLabel(const CRpgEvent& com)
{
	currentCommandIndex_ = labels_[com.getIntParam(0) - 1];		// 戻り先で+1されるのでちょうどLabelの次になる
}

void GameEventManager::comOperatePartyChange(const CRpgEvent& com)
{
	GameSystem& system = gameField_->getGameSystem();
	int playerId = com.getIntParam(1) == 0? com.getIntParam(2) : system.getVar(com.getIntParam(2));
	if (com.getIntParam(0) == 0) {		// メンバーを加える
		gameField_->addPlayer(playerId);
	} else {							// メンバーを外す
		gameField_->removePlayer(playerId);
	}
}

void GameEventManager::comOperateLocateMove(const CRpgEvent& com)
{
	int dir = com.getIntParamNum() >= 4? com.getIntParam(3) : 0;
	gameField_->changeMap(com.getIntParam(0), com.getIntParam(1), com.getIntParam(2), dir);
	waitEventInfo_.enable = true;
}

void GameEventManager::comWaitLocateMove(const CRpgEvent& com)
{
	waitEventInfo_.enable = false;
}

void GameEventManager::comOperateLocateSave(const CRpgEvent& com)
{
	GameSystem& system = gameField_->getGameSystem();
	system.setVar(com.getIntParam(0), gameField_->getMap()->getMapId());
	system.setVar(com.getIntParam(1), gameField_->getPlayerLeader()->getPosition().x);
	system.setVar(com.getIntParam(2), gameField_->getPlayerLeader()->getPosition().y);
}

void GameEventManager::comOperateLocateLoad(const CRpgEvent& com)
{
	GameSystem& system = gameField_->getGameSystem();
	system.setVar(com.getIntParam(0), gameField_->getMap()->getMapId());
	system.setVar(com.getIntParam(1), gameField_->getPlayerLeader()->getPosition().x);
	system.setVar(com.getIntParam(2), gameField_->getPlayerLeader()->getPosition().y);
	gameField_->changeMap(system.getVar(com.getIntParam(0)), system.getVar(com.getIntParam(1)), system.getVar(com.getIntParam(2)), 0);
	waitEventInfo_.enable = true;
}

void GameEventManager::comOperateTextShow(const CRpgEvent& com)
{
	openGameMassageWindow();
	gameMessageWindow_->addMessage(com.getStringParam());
	for (uint i = currentCommandIndex_ + 1; i < currentEventPage_->events.size(); i++) {
		const CRpgEvent& comNext = currentEventPage_->events[i];
		if (comNext.getEventCode() == CODE_TXT_SHOW_ADD) {
			gameMessageWindow_->addMessage(comNext.getStringParam());
		} else {
			break;
		}
	}
	
	waitEventInfo_.enable = true;
}

void GameEventManager::openGameMassageWindow()
{
	gameMessageWindow_->freeze(false);
	gameMessageWindow_->open();
	gameMessageWindow_->reset();
	gameMessageWindow_->clearMessages();
	gameMessageWindow_->setShowFrame(messageWindowSetting_.showFrame);
	switch (messageWindowSetting_.pos) {
	case MessageWindowSetting::kPosTypeUp:
		gameMessageWindow_->setPosition(kuto::Vector2(0.f, 0.f));
		break;
	case MessageWindowSetting::kPosTypeCenter:
		gameMessageWindow_->setPosition(kuto::Vector2(0.f, 80.f));
		break;
	default:
		gameMessageWindow_->setPosition(kuto::Vector2(0.f, 160.f));
		break;
	}
	if (messageWindowSetting_.autoMove) {
		// Undefined
	}
}

void GameEventManager::comWaitTextShow(const CRpgEvent& com)
{
	if (gameMessageWindow_->closed()) {
		waitEventInfo_.enable = false;
		gameMessageWindow_->freeze(true);
	}
}

void GameEventManager::comOperateTextOption(const CRpgEvent& com)
{
	messageWindowSetting_.showFrame = !com.getIntParam(0);
	messageWindowSetting_.pos = (MessageWindowSetting::PosType)com.getIntParam(1);
	messageWindowSetting_.autoMove = com.getIntParam(2);
	messageWindowSetting_.enableOtherEvent = com.getIntParam(3);
}

void GameEventManager::comOperateTextFace(const CRpgEvent& com)
{
	gameMessageWindow_->setFaceTexture(com.getStringParam(), (u8)com.getIntParam(0), (bool)com.getIntParam(1), (bool)com.getIntParam(2));
	selectWindow_->setFaceTexture(com.getStringParam(), (u8)com.getIntParam(0), (bool)com.getIntParam(1), (bool)com.getIntParam(2));
}

void GameEventManager::comOperateBattleStart(const CRpgEvent& com)
{
	GameSystem& system = gameField_->getGameSystem();
	std::string terrain;
	if (com.getIntParam(2) == 0) {
		int terrainId = gameField_->getMap()->getTerrainId(eventPageInfos_[currentEventIndex_].x, eventPageInfos_[currentEventIndex_].y);
		terrain = system.getRpgLdb().saTerrain[terrainId].battleGraphic;
	} else {
		terrain = com.getStringParam();
	}
	int enemyId = com.getIntParam(0) == 0? com.getIntParam(1) : system.getVar(com.getIntParam(1));
	gameField_->startBattle(terrain, enemyId, (bool)com.getIntParam(5), com.getIntParam(3) != 0, com.getIntParam(4) == 0);
	waitEventInfo_.enable = true;
}

void GameEventManager::comWaitBattleStart(const CRpgEvent& com)
{
	waitEventInfo_.enable = false;
	int result = gameField_->getBattleResult();
	if (com.getIntParam(3) == 1 && result == GameBattle::kResultEscape) {
		comOperateEventBreak(com);		// Escape -> Break
	} else if (com.getIntParam(3) == 2 || com.getIntParam(4) == 1) {
		conditionStack_.push(ConditionInfo(com.getNest(), result));
	}
}

void GameEventManager::comOperateBattleWin(const CRpgEvent& com)
{
	executeChildCommands_ = conditionStack_.top().value == GameBattle::kResultWin;
}

void GameEventManager::comOperateBattleEscape(const CRpgEvent& com)
{
	executeChildCommands_ = conditionStack_.top().value == GameBattle::kResultEscape;
}

void GameEventManager::comOperateBattleLose(const CRpgEvent& com)
{
	executeChildCommands_ = conditionStack_.top().value == GameBattle::kResultLose;
}

void GameEventManager::comOperateBranchEnd(const CRpgEvent& com)
{
	conditionStack_.pop();
	executeChildCommands_ = true;
}

void GameEventManager::comOperateIfStart(const CRpgEvent& com)
{
	GameSystem& system = gameField_->getGameSystem();
	int condValue = 0;
	switch (com.getIntParam(0)) {
	case 0:		// 0:スイッチ
		condValue = system.getSwitch(com.getIntParam(1)) == (com.getIntParam(2) == 0? true : false);
		break;
	case 1:		// 1:変数
		{
			int value = com.getIntParam(2) == 0? com.getIntParam(3) : system.getVar(com.getIntParam(3));
			switch (com.getIntParam(4)) {
			case 0:		// と同値
				condValue = (system.getVar(com.getIntParam(1)) == value);
				break;
			case 1:		// 	以上
				condValue = (system.getVar(com.getIntParam(1)) >= value);
				break;
			case 2:		// 	以下
				condValue = (system.getVar(com.getIntParam(1)) <= value);
				break;
			case 3:		// より大きい
				condValue = (system.getVar(com.getIntParam(1)) > value);
				break;
			case 4:		// より小さい
				condValue = (system.getVar(com.getIntParam(1)) < value);
				break;
			case 5:		// 以外
				condValue = (system.getVar(com.getIntParam(1)) != value);
				break;
			}
		}
		break;
	case 2:		// 2:タイマー1
		switch (com.getIntParam(2)) {
		case 0:		// 以上
			condValue = (timer_.enable? timer_.count >= com.getIntParam(1) * 60 : false);
			break;
		case 1:		// 以下
			condValue = (timer_.enable? timer_.count <= com.getIntParam(1) * 60 : false);
			break;
		}
		break;
	case 3:		// 3:所持金
		switch (com.getIntParam(2)) {
		case 0:		// 以上
			condValue = (system.getInventory()->getMoney() >= com.getIntParam(1));
			break;
		case 1:		// 以下
			condValue = (system.getInventory()->getMoney() <= com.getIntParam(1));
			break;
		}
		break;
	case 4:		// 4:アイテム
		switch (com.getIntParam(2)) {
		case 0:		// 持っていない
			condValue = (system.getInventory()->getItemNum(com.getIntParam(1)) == 0);
			break;
		case 1:		// 持っている
			condValue = (system.getInventory()->getItemNum(com.getIntParam(1)) > 0);
			break;
		}
		break;
	case 5:		// 5:主人公
		switch (com.getIntParam(2)) {
		case 0:		// パーティにいる
			condValue = gameField_->getPlayerFromId(com.getIntParam(1)) != NULL;
			break;
		case 1:		// 主人公の名前が文字列引数と等しい
			condValue = com.getStringParam() == gameField_->getGameSystem().getPlayerInfo(com.getIntParam(1)).name;
			break;
		case 2:		// レベルがCの値以上
			condValue = gameField_->getGameSystem().getPlayerStatus(com.getIntParam(1)).getLevel() >= com.getIntParam(3);
			break;
		case 3:		// HPがCの値以上
			condValue = gameField_->getGameSystem().getPlayerStatus(com.getIntParam(1)).getHp() >= com.getIntParam(3);
			break;
		case 4:		// 特殊技能IDがCの値の特殊技能を使用できる
			condValue = gameField_->getGameSystem().getPlayerStatus(com.getIntParam(1)).isLearnedSkill(com.getIntParam(3));
			break;
		case 5:		// アイテムIDがCの値のアイテムを装備している
			condValue = gameField_->getGameSystem().getPlayerStatus(com.getIntParam(1)).getEquip().weapon == com.getIntParam(3) ||
				gameField_->getGameSystem().getPlayerStatus(com.getIntParam(1)).getEquip().shield == com.getIntParam(3) ||
				gameField_->getGameSystem().getPlayerStatus(com.getIntParam(1)).getEquip().protector == com.getIntParam(3) ||
				gameField_->getGameSystem().getPlayerStatus(com.getIntParam(1)).getEquip().helmet == com.getIntParam(3) ||
				gameField_->getGameSystem().getPlayerStatus(com.getIntParam(1)).getEquip().accessory == com.getIntParam(3);
			break;
		case 6:		// 状態IDがCの状態になっている
			condValue = gameField_->getGameSystem().getPlayerStatus(com.getIntParam(1)).getBadConditionIndex(com.getIntParam(3)) >= 0;
			break;
		}
		break;
	case 6:		// 6:キャラの向き
		{
			GameChara* chara = getCharaFromEventId(com.getIntParam(1));
			if (chara) {
				condValue = chara->getDirection() == com.getIntParam(2);
			} else
				condValue = false;
		}
		break;
	case 7:		// 7:乗り物
		condValue = (false);	// Undefined
		break;
	case 8:		// 8:決定キーでこのイベントを開始した
		condValue = startDecideButton_;
		break;
	case 9:		// 9:演奏中のBGMが一周した
		condValue = bgm_->isLooped();	// Undefined
		break;
	}
	conditionStack_.push(ConditionInfo(com.getNest(), condValue));
	executeChildCommands_ = conditionStack_.top().value == true;
}

void GameEventManager::comOperateIfElse(const CRpgEvent& com)
{
	executeChildCommands_ = conditionStack_.top().value == false;
}

void GameEventManager::comOperateSelectStart(const CRpgEvent& com)
{
	selectWindow_->freeze(false);
	selectWindow_->open();
	selectWindow_->reset();
	selectWindow_->resetCursor();
	selectWindow_->clearMessages();
	selectWindow_->setShowFrame(messageWindowSetting_.showFrame);
	switch (messageWindowSetting_.pos) {
	case MessageWindowSetting::kPosTypeUp:
		selectWindow_->setPosition(kuto::Vector2(0.f, 0.f));
		break;
	case MessageWindowSetting::kPosTypeCenter:
		selectWindow_->setPosition(kuto::Vector2(0.f, 80.f));
		break;
	default:
		selectWindow_->setPosition(kuto::Vector2(0.f, 160.f));
		break;
	}
	if (messageWindowSetting_.autoMove) {
		// Undefined
	}
	std::string::size_type oldPos = 0;
	for (int i = 0; i < 4; i++) {
		std::string::size_type pos = com.getStringParam().find('/', oldPos);
		if (pos == std::string::npos) {
			selectWindow_->addMessage(com.getStringParam().substr(oldPos));
			break;
		} else {
			selectWindow_->addMessage(com.getStringParam().substr(oldPos, pos - oldPos));
			oldPos = pos + 1;
		}
	}
	selectWindow_->setEnableCancel(com.getIntParam(0) != 0);
	
	waitEventInfo_.enable = true;
}

void GameEventManager::comWaitSelectStart(const CRpgEvent& com)
{
	if (selectWindow_->closed()) {
		waitEventInfo_.enable = false;
		selectWindow_->freeze(true);
		int selectIndex = selectWindow_->cursor();
		if (selectWindow_->canceled()) {
			selectIndex = com.getIntParam(0) - 1;
		}
		int value = selectIndex < (int)selectWindow_->getMessageSize()? kuto::crc32(selectWindow_->getMessage(selectIndex).str) : 0;
		conditionStack_.push(ConditionInfo(com.getNest(), value));
	}
}

void GameEventManager::comOperateSelectCase(const CRpgEvent& com)
{
	executeChildCommands_ = (uint)conditionStack_.top().value == kuto::crc32(com.getStringParam());
}

void GameEventManager::comOperateGameOver(const CRpgEvent& com)
{
	gameField_->gameOver();
	waitEventInfo_.enable = true;
}

void GameEventManager::comOperateReturnTitle(const CRpgEvent& com)
{
	gameField_->returnTitle();
	waitEventInfo_.enable = true;
}

void GameEventManager::comOperateEventBreak(const CRpgEvent& com)
{
	currentCommandIndex_ = 100000;		// 100000行もないだろ
}

void GameEventManager::comOperateEventClear(const CRpgEvent& com)
{
	eventPageInfos_[currentEventIndex_].cleared = true;
	if (eventPageInfos_[currentEventIndex_].npc) {
		eventPageInfos_[currentEventIndex_].npc->freeze(true);
	}
}

void GameEventManager::comOperateLoopStart(const CRpgEvent& com)
{
	LoopInfo info;
	info.startIndex = currentCommandIndex_;
	info.conditionSize = conditionStack_.size();
	for (uint i = currentCommandIndex_ + 1; i < currentEventPage_->events.size(); i++) {
		const CRpgEvent& comNext = currentEventPage_->events[i];
		if (com.getNest() != comNext.getNest()) {
			continue;
		}
		if (comNext.getEventCode() == CODE_LOOP_END) {
			info.endIndex = i;
			break;
		}
	}
	loopStack_.push(info);
}

void GameEventManager::comOperateLoopBreak(const CRpgEvent& com)
{
	while (loopStack_.top().conditionSize < (int)conditionStack_.size()) {
		conditionStack_.pop();
	}
	currentCommandIndex_ = loopStack_.top().endIndex;		// 戻り先で+1されるのでちょうどLoopStartの次になる
	loopStack_.pop();
}

void GameEventManager::comOperateLoopEnd(const CRpgEvent& com)
{
	currentCommandIndex_ = loopStack_.top().startIndex;		// 戻り先で+1されるのでちょうどLoopStartの次になる
}

void GameEventManager::comOperateWait(const CRpgEvent& com)
{
	waitEventInfo_.enable = true;
}

void GameEventManager::comWaitWait(const CRpgEvent& com)
{
	waitEventInfo_.count++;
	if (waitEventInfo_.count * 60 > com.getIntParam(0) * 60 / 10) {
		waitEventInfo_.enable = false;
	}
}

static void setPictureInfo(GameEventPicture::Info& info, const CRpgEvent& com, GameField* field)
{
	GameSystem& system = field->getGameSystem();
	info.map = field->getMap();
	info.position.x = com.getIntParam(1) == 0? com.getIntParam(2) : system.getVar(com.getIntParam(2));
	info.position.y = com.getIntParam(1) == 0? com.getIntParam(3) : system.getVar(com.getIntParam(3));
	info.scroll = (bool)com.getIntParam(4);
	info.scale = (float)com.getIntParam(5) / 100.f;
	info.color.a = 1.f - (float)com.getIntParam(6) / 100.f;
	info.useAlpha = (bool)com.getIntParam(7);
	info.color.r = (float)com.getIntParam(8) / 100.f;
	info.color.g = (float)com.getIntParam(9) / 100.f;
	info.color.b = (float)com.getIntParam(10) / 100.f;
	info.saturation = (float)com.getIntParam(11) / 100.f;
	info.effect = (GameEventPicture::EffectType)com.getIntParam(12);
	info.effectSpeed = com.getIntParam(13);
}

void GameEventManager::comOperatePictureShow(const CRpgEvent& com)
{
	GameSystem& system = gameField_->getGameSystem();
	int picIndex = com.getIntParam(0) - 1;
	if (pictures_[picIndex]) {
		pictures_[picIndex]->release();
		pictures_[picIndex] = NULL;
	}
	GameEventPicture::Info info;
	setPictureInfo(info, com, gameField_);
	
	std::string filename = system.getRootFolder();
	filename += "/Picture/" + com.getStringParam();
	pictures_[picIndex] = GameEventPicture::createTask(this, filename, info);
}

void GameEventManager::comOperatePictureMove(const CRpgEvent& com)
{
	int picIndex = com.getIntParam(0) - 1;
	if (!pictures_[picIndex]) {
		return;
	}
	GameEventPicture::Info info;
	setPictureInfo(info, com, gameField_);
	
	pictures_[picIndex]->move(info, com.getIntParam(14) * 60 / 10);
	waitEventInfo_.enable = (bool)com.getIntParam(15);
}

void GameEventManager::comWaitPictureMove(const CRpgEvent& com)
{
	int picIndex = com.getIntParam(0) - 1;
	if (!pictures_[picIndex]->isMoving()) {
		waitEventInfo_.enable = false;
	}
}

void GameEventManager::comOperatePictureClear(const CRpgEvent& com)
{
	int picIndex = com.getIntParam(0) - 1;
	if (pictures_[picIndex]) {
		pictures_[picIndex]->release();
		pictures_[picIndex] = NULL;
	}
}

void GameEventManager::comOperateFadeType(const CRpgEvent& com)
{
	gameField_->setFadeInfo(com.getIntParam(0), com.getIntParam(1));
}

void GameEventManager::comOperateFadeOut(const CRpgEvent& com)
{
	gameField_->fadeOut(com.getIntParam(0));
}

void GameEventManager::comOperateFadeIn(const CRpgEvent& com)
{
	gameField_->fadeIn(com.getIntParam(0));
}

void GameEventManager::comOperateMapScroll(const CRpgEvent& com)
{
	GameMap* map = gameField_->getMap();
	if (com.getIntParam(0) < 2)
		map->setEnableScroll((bool)com.getIntParam(0));
	else {
		if (com.getIntParam(0) == 2) {
			int x = 0;
			int y = 0;
			if (com.getIntParam(1) == 0)	// 上
				y = -com.getIntParam(2);
			else if (com.getIntParam(1) == 1)	// 右
				x = com.getIntParam(2);
			else if (com.getIntParam(1) == 2)	// 下
				y = com.getIntParam(2);
			else if (com.getIntParam(1) == 3)	// 左
				x = -com.getIntParam(2);
			map->scroll(x, y, (float)(1 << com.getIntParam(3)));
		} else
			map->scrollBack((float)(1 << com.getIntParam(3)));
		if (com.getIntParam(4) == 1);
			waitEventInfo_.enable = true;
	}
}

void GameEventManager::comWaitMapScroll(const CRpgEvent& com)
{
	if (!gameField_->getMap()->isScrolling())
		waitEventInfo_.enable = false;
}

void GameEventManager::comOperatePlayerVisible(const CRpgEvent& com)
{
	gameField_->getPlayerLeader()->setVisible((bool)com.getIntParam(0));
}

void GameEventManager::addLevelUpMessage(const GameCharaStatus& status, int oldLevel)
{
	GameSystem& system = gameField_->getGameSystem();
	const CRpgLdb::Term& term = system.getRpgLdb().term;
	const GamePlayerInfo& player = system.getPlayerInfo(status.getCharaId());
	char temp[256];
	sprintf(temp, "%sは%s%d%s", player.name.c_str(), term.param.level.c_str(),
		status.getLevel(), term.battle.levelUp.c_str());
	gameMessageWindow_->addMessage(temp);
	for (u32 iLearn = 1; iLearn < player.baseInfo->learnSkill.size(); iLearn++) {
		const CRpgLdb::LearnSkill& learnSkill = player.baseInfo->learnSkill[iLearn];
		if (learnSkill.level > oldLevel && learnSkill.level <= status.getLevel()) {
			const CRpgLdb::Skill& skill = system.getRpgLdb().saSkill[learnSkill.skill];
			sprintf(temp, "%s%s", skill.name.c_str(), term.battle.getSkill.c_str());
			gameMessageWindow_->addMessage(temp);
		}
	}			
}

void GameEventManager::comOperatePlayerCure(const CRpgEvent& com)
{
	GameSystem& system = gameField_->getGameSystem();
	kuto::StaticVector<GameCharaStatus*, 4> statusList;
	switch (com.getIntParam(0)) {
	case 0:		// 0:パーティーメンバー全員
		for (u32 i = 0; i < gameField_->getPlayers().size(); i++) {
			statusList.push_back(&gameField_->getPlayers()[i]->getStatus());
		}
		break;
	case 1:		// 1:[固定] 主人公IDがAの主人公
		statusList.push_back(&system.getPlayerStatus(com.getIntParam(1)));
		break;
	case 2:		// 2:[変数] 主人公IDがV[A]の主人公
		statusList.push_back(&system.getPlayerStatus(system.getVar(com.getIntParam(1))));
		break;
	}
	for (u32 i = 0; i < statusList.size(); i++) {
		statusList[i]->fullCure();
	}
}

void GameEventManager::comOperateAddExp(const CRpgEvent& com)
{
	GameSystem& system = gameField_->getGameSystem();
	kuto::StaticVector<GameCharaStatus*, 4> statusList;
	switch (com.getIntParam(0)) {
	case 0:		// 0:パーティーメンバー全員
		for (u32 i = 0; i < gameField_->getPlayers().size(); i++) {
			statusList.push_back(&gameField_->getPlayers()[i]->getStatus());
		}
		break;
	case 1:		// 1:[固定] 主人公IDがAの主人公
		statusList.push_back(&system.getPlayerStatus(com.getIntParam(1)));
		break;
	case 2:		// 2:[変数] 主人公IDがV[A]の主人公
		statusList.push_back(&system.getPlayerStatus(system.getVar(com.getIntParam(1))));
		break;
	}
	int exp = com.getIntParam(3) == 0? com.getIntParam(4) : system.getVar(com.getIntParam(4));
	kuto::StaticVector<std::pair<GameCharaStatus*, int>, 4> levelUpList;
	for (u32 i = 0; i < statusList.size(); i++) {
		int oldLevel = statusList[i]->getLevel();
		statusList[i]->addExp(com.getIntParam(2) == 0? exp : -exp);
		if (statusList[i]->getLevel() > oldLevel) {
			levelUpList.push_back(std::make_pair(statusList[i], oldLevel));
		}
	}
	if (com.getIntParam(5) == 1 && !levelUpList.empty()) {
		openGameMassageWindow();
		for (u32 i = 0; i < levelUpList.size(); i++) {
			addLevelUpMessage(*levelUpList[i].first, levelUpList[i].second);
		}
		waitEventInfo_.enable = true;
	}
}

void GameEventManager::comOperateAddLevel(const CRpgEvent& com)
{
	GameSystem& system = gameField_->getGameSystem();
	kuto::StaticVector<GameCharaStatus*, 4> statusList;
	switch (com.getIntParam(0)) {
	case 0:		// 0:パーティーメンバー全員
		for (u32 i = 0; i < gameField_->getPlayers().size(); i++) {
			statusList.push_back(&gameField_->getPlayers()[i]->getStatus());
		}
		break;
	case 1:		// 1:[固定] 主人公IDがAの主人公
		statusList.push_back(&system.getPlayerStatus(com.getIntParam(1)));
		break;
	case 2:		// 2:[変数] 主人公IDがV[A]の主人公
		statusList.push_back(&system.getPlayerStatus(system.getVar(com.getIntParam(1))));
		break;
	}
	int level = com.getIntParam(3) == 0? com.getIntParam(4) : system.getVar(com.getIntParam(4));
	kuto::StaticVector<std::pair<GameCharaStatus*, int>, 4> levelUpList;
	for (u32 i = 0; i < statusList.size(); i++) {
		int oldLevel = statusList[i]->getLevel();
		statusList[i]->addLevel(com.getIntParam(2) == 0? level : -level);
		if (statusList[i]->getLevel() > oldLevel) {
			levelUpList.push_back(std::make_pair(statusList[i], oldLevel));
		}
	}
	if (com.getIntParam(5) == 1 && !levelUpList.empty()) {
		openGameMassageWindow();
		for (u32 i = 0; i < levelUpList.size(); i++) {
			addLevelUpMessage(*levelUpList[i].first, levelUpList[i].second);
		}
		waitEventInfo_.enable = true;
	}
}

void GameEventManager::comOperateAddStatus(const CRpgEvent& com)
{
	GameSystem& system = gameField_->getGameSystem();
	kuto::StaticVector<GameCharaStatus*, 4> statusList;
	switch (com.getIntParam(0)) {
	case 0:		// 0:パーティーメンバー全員
		for (u32 i = 0; i < gameField_->getPlayers().size(); i++) {
			statusList.push_back(&gameField_->getPlayers()[i]->getStatus());
		}
		break;
	case 1:		// 1:[固定] 主人公IDがAの主人公
		statusList.push_back(&system.getPlayerStatus(com.getIntParam(1)));
		break;
	case 2:		// 2:[変数] 主人公IDがV[A]の主人公
		statusList.push_back(&system.getPlayerStatus(system.getVar(com.getIntParam(1))));
		break;
	}
	int upParam = com.getIntParam(4) == 0? com.getIntParam(5) : system.getVar(com.getIntParam(5));
	for (u32 i = 0; i < statusList.size(); i++) {
		CRpgLdb::Status itemUp;
		std::memset(&itemUp, 0, sizeof(itemUp));
		switch (com.getIntParam(3)) {
		case 0:		// 0:最大HP
			itemUp.maxHP = com.getIntParam(2) == 0? upParam : -upParam;
			break;
		case 1:		// 1:最大MP
			itemUp.maxMP = com.getIntParam(2) == 0? upParam : -upParam;
			break;
		case 2:		// 2:攻撃力
			itemUp.attack = com.getIntParam(2) == 0? upParam : -upParam;
			break;
		case 3:		// 3:防御力
			itemUp.defence = com.getIntParam(2) == 0? upParam : -upParam;
			break;
		case 4:		// 4:精神力
			itemUp.magic = com.getIntParam(2) == 0? upParam : -upParam;
			break;
		case 5:		// 5:敏捷性
			itemUp.speed = com.getIntParam(2) == 0? upParam : -upParam;
			break;
		}
		statusList[i]->addItemUp(itemUp);
	}
}

void GameEventManager::comOperateAddSkill(const CRpgEvent& com)
{
	GameSystem& system = gameField_->getGameSystem();
	kuto::StaticVector<GameCharaStatus*, 4> statusList;
	switch (com.getIntParam(0)) {
	case 0:		// 0:パーティーメンバー全員
		for (u32 i = 0; i < gameField_->getPlayers().size(); i++) {
			statusList.push_back(&gameField_->getPlayers()[i]->getStatus());
		}
		break;
	case 1:		// 1:[固定] 主人公IDがAの主人公
		statusList.push_back(&system.getPlayerStatus(com.getIntParam(1)));
		break;
	case 2:		// 2:[変数] 主人公IDがV[A]の主人公
		statusList.push_back(&system.getPlayerStatus(system.getVar(com.getIntParam(1))));
		break;
	}
	int skillId = com.getIntParam(3) == 0? com.getIntParam(4) : system.getVar(com.getIntParam(4));
	for (u32 i = 0; i < statusList.size(); i++) {
		if (com.getIntParam(2) == 0)
			statusList[i]->learnSkill(skillId);
		else
			statusList[i]->forgetSkill(skillId);
	}
}

void GameEventManager::comOperateCallEvent(const CRpgEvent& com)
{
	const CRpgLmu& rpgLmu = gameField_->getMap()->getRpgLmu();
	GameSystem& system = gameField_->getGameSystem();
	// backup
	{
		CallEventInfo info;
		info.eventIndex = currentEventIndex_;
		info.executeChildCommands = executeChildCommands_;
		info.page = currentEventPage_;
		std::memcpy(info.labels.get(), labels_.get(), labels_.size() * sizeof(int));
		info.pos = currentCommandIndex_;
		info.conditionStack = conditionStack_;
		info.loopStack = loopStack_;
		callStack_.push(info);
	}
	// call
	executeChildCommands_ = false;
	conditionStack_.clear();
	loopStack_.clear();
	if (com.getIntParam(0) == 0) {
		// common event
		int eventId = com.getIntParam(1);
		currentEventIndex_ = eventId + rpgLmu.saMapEvent.GetSize();
		executeCommands(system.getRpgLdb().saCommonEvent[eventId].eventList, 0);
	} else {
		// map event
		int eventId = (com.getIntParam(0) == 1)? com.getIntParam(1) : system.getVar(com.getIntParam(1));
		int eventPage = (com.getIntParam(0) == 1)? com.getIntParam(2) : system.getVar(com.getIntParam(2));
		if (eventId == 10005)
			eventId = currentEventIndex_;
		currentEventIndex_ = eventId;
		executeCommands(rpgLmu.saMapEvent[eventId].saPage[eventPage].eventList, 0);
	}
	if (waitEventInfo_.enable) {
		backupWaitInfoEnable_ = true;
	} else {
		// restore
		restoreCallStack();
	}
}

void GameEventManager::restoreCallStack()
{
	const CallEventInfo& info = callStack_.top();
	currentEventIndex_ = info.eventIndex;
	executeChildCommands_ = info.executeChildCommands;
	currentEventPage_ = info.page;
	std::memcpy(labels_.get(), info.labels.get(), labels_.size() * sizeof(int));
	currentCommandIndex_ = info.pos;
	conditionStack_ = info.conditionStack;
	loopStack_ = info.loopStack;
	callStack_.pop();
}

void GameEventManager::comOperateRoute(const CRpgEvent& com)
{
	GameChara* chara = getCharaFromEventId(com.getIntParam(0));
	if (!chara)
		return;
	// int frequency = com.getIntParam(1);
	CRpgRoute route;
	route.repeat = com.getIntParam(2) == 1;
	route.ignore = com.getIntParam(3) == 1;
	for (int i = 4; i < com.getIntParamNum(); i++) {
		route.commands.push_back(com.getIntParam(i));
		switch (com.getIntParam(i)) {
		case 32:	// スイッチON
		case 33:	// スイッチOFF
			//route.extraIntParam.push_back(com.getExtraIntParam(i, 0));
			break;
		case 34:	// グラフィック変更
			//route.extraStringParam.push_back(com.getExtraStringParam(i, 0));
			//route.extraIntParam.push_back(com.getExtraIntParam(i, 0));
			break;
		case 35:	// 効果音の演奏
			//route.extraStringParam.push_back(com.getExtraStringParam(i, 0));
			//route.extraIntParam.push_back(com.getExtraIntParam(i, 0));
			//route.extraIntParam.push_back(com.getExtraIntParam(i, 1));
			//route.extraIntParam.push_back(com.getExtraIntParam(i, 2));
			break;
		}
	}
	chara->setRoute(route);
	routeSetChara_ = chara;
	routeSetChara_->startRoute();
}

void GameEventManager::comOperateRouteStart(const CRpgEvent& com)
{
	if (!routeSetChara_)
		return;
	routeSetChara_->startRoute();
}

void GameEventManager::comOperateRouteEnd(const CRpgEvent& com)
{
	if (!routeSetChara_)
		return;
	routeSetChara_->endRoute();
}

void GameEventManager::comOperateNameInput(const CRpgEvent& com)
{
	nameInputMenu_->freeze(false);
	nameInputMenu_->setPlayerInfo(com.getIntParam(0), (bool)com.getIntParam(1), (bool)com.getIntParam(2));
	waitEventInfo_.enable = true;
}

void GameEventManager::comWaitNameInput(const CRpgEvent& com)
{
	if (nameInputMenu_->closed()) {
		waitEventInfo_.enable = false;
		nameInputMenu_->freeze(true);
	}
}

void GameEventManager::comOperatePlayerNameChange(const CRpgEvent& com)
{
	GameSystem& system = gameField_->getGameSystem();
	system.getPlayerInfo(com.getIntParam(0)).name = com.getStringParam();
}

void GameEventManager::comOperatePlayerTitleChange(const CRpgEvent& com)
{
	GameSystem& system = gameField_->getGameSystem();
	system.getPlayerInfo(com.getIntParam(0)).title = com.getStringParam();
}

void GameEventManager::comOperatePlayerWalkChange(const CRpgEvent& com)
{
	GameSystem& system = gameField_->getGameSystem();
	int playerId = com.getIntParam(0);
	GamePlayerInfo& playerInfo = system.getPlayerInfo(playerId);
	playerInfo.walkGraphicName = com.getStringParam();
	playerInfo.walkGraphicPos = com.getIntParam(1);
	playerInfo.walkGraphicSemi = (bool)com.getIntParam(2);
	
	GamePlayer* player = gameField_->getPlayerFromId(playerId);
	if (player) {
		player->loadWalkTexture(playerInfo.walkGraphicName, playerInfo.walkGraphicPos);
	}
}

void GameEventManager::comOperatePlayerFaceChange(const CRpgEvent& com)
{
	GameSystem& system = gameField_->getGameSystem();
	int playerId = com.getIntParam(0);
	GamePlayerInfo& playerInfo = system.getPlayerInfo(playerId);
	playerInfo.faceGraphicName = com.getStringParam();
	playerInfo.faceGraphicPos = com.getIntParam(1);

	GamePlayer* player = gameField_->getPlayerFromId(playerId);
	if (player) {
		player->loadFaceTexture(playerInfo.faceGraphicName, playerInfo.faceGraphicPos);
	}
}

void GameEventManager::comOperateBgm(const CRpgEvent& com)
{
	// Undefined
}

int getInputKeyValue(const CRpgEvent& com)
{
	kuto::VirtualPad* virtualPad = kuto::VirtualPad::instance();
	int key = 0;
	if (com.getIntParam(2) == 1) {
		if (virtualPad->press(kuto::VirtualPad::KEY_DOWN))
			key = 1;
		if (virtualPad->press(kuto::VirtualPad::KEY_LEFT))
			key = 2;
		if (virtualPad->press(kuto::VirtualPad::KEY_RIGHT))
			key = 3;
		if (virtualPad->press(kuto::VirtualPad::KEY_UP))
			key = 4;
	} else {
		if (com.getIntParam(5) == 1 && virtualPad->press(kuto::VirtualPad::KEY_X))
			key = 1;
		if (com.getIntParam(6) == 1 && virtualPad->press(kuto::VirtualPad::KEY_DOWN))
			key = 1;
		if (com.getIntParam(7) == 1 && virtualPad->press(kuto::VirtualPad::KEY_LEFT))
			key = 2;
		if (com.getIntParam(8) == 1 && virtualPad->press(kuto::VirtualPad::KEY_RIGHT))
			key = 3;
		if (com.getIntParam(9) == 1 && virtualPad->press(kuto::VirtualPad::KEY_UP))
			key = 4;
	}
	if (com.getIntParam(3) == 1 && virtualPad->press(kuto::VirtualPad::KEY_A))
		key = 5;
	if (com.getIntParam(4) == 1 && virtualPad->press(kuto::VirtualPad::KEY_B))
		key = 6;
	return key;
}

void GameEventManager::comOperateKey(const CRpgEvent& com)
{
	GameSystem& system = gameField_->getGameSystem();
	waitEventInfo_.enable = com.getIntParam(1);
	if (!waitEventInfo_.enable)
		system.setVar(com.getIntParam(0), getInputKeyValue(com));
}

void GameEventManager::comWaitKey(const CRpgEvent& com)
{
	int key = getInputKeyValue(com);
	if (key != 0) {
		waitEventInfo_.enable = false;
		GameSystem& system = gameField_->getGameSystem();
		system.setVar(com.getIntParam(0), key);
	}
}

void GameEventManager::comOperatePanorama(const CRpgEvent& com)
{
	CRpgLmu::PanoramaInfo info;
	info.enable = true;
	info.name = com.getStringParam();
	info.loopHorizontal = (com.getIntParam(0) == 1);
	info.loopVertical = (com.getIntParam(1) == 1);
	info.scrollHorizontal = (com.getIntParam(2) == 1);
	info.scrollSpeedHorizontal = com.getIntParam(3);
	info.scrollVertical = (com.getIntParam(4) == 1);
	info.scrollSpeedVertical = com.getIntParam(5);
	gameField_->getMap()->getRpgLmu().SetPanoramaInfo(info);
}



void GameEventManager::draw()
{
	if (timer_.draw) {
		// Undefined	
	}
}
