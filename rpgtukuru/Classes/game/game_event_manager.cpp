/**
 * @file
 * @brief Game Event Manager
 * @author project.kuto
 */

#include <deque>
#include <sstream>
#include <utility>

#include <kuto/kuto_utility.h>
#include <kuto/kuto_error.h>
#include <kuto/kuto_virtual_pad.h>

#include <rpg2k/Debug.hpp>

#include "game.h"
#include "game_battle.h"
#include "game_bgm.h"
#include "game_collision.h"
#include "game_event_manager.h"
#include "game_event_map_chip.h"
#include "game_event_picture.h"
#include "game_field.h"
#include "game_inventory.h"
#include "game_map.h"
#include "game_message_window.h"
#include "game_name_input_menu.h"
#include "game_npc.h"
#include "game_player.h"
#include "game_select_window.h"
#include "game_shop_menu.h"
#include "game_skill_anime.h"

using rpg2k::structure::Array1D;
using rpg2k::structure::Array2D;


#include "game_event_command.h"
template<int CODE>
void GameEventManager::addCommand()
{
	bool const res = comFuncMap_.insert( std::make_pair(
		CODE, &GameEventManager::command<CODE> ) ).second;
	kuto_assert(res);
}
template<int CODE>
void GameEventManager::addCommandWait()
{
	bool const res = comWaitFuncMap_.insert( std::make_pair(
		CODE, &GameEventManager::commandWait<CODE> ) ).second;
	kuto_assert(res);
}

GameEventManager::GameEventManager(GameField* field)
: kuto::Task()
, gameField_(field)
, currentEventPage_(NULL), executeChildCommands_(true), encountStep_(0), routeSetChara_(NULL)
, bgm_(NULL), skillAnime_(NULL)
{
	bgm_ = addChild( GameBgm::createTask(NULL) );	// temp
	// const rpg2k::model::MapUnit& rpgLmu = gameField_->getMap()->getRpgLmu();
	initEventPageInfos();
	updateEventAppear();

	gameMessageWindow_ = addChild(GameMessageWindow::createTask(gameField_->getGameSystem()));
	gameMessageWindow_->setPosition(kuto::Vector2(0.f, 160.f));
	gameMessageWindow_->setSize(kuto::Vector2(320.f, 80.f));
	gameMessageWindow_->setMessageAlign(GameMessageWindow::kAlignLeft);
	gameMessageWindow_->pauseUpdate(true);

	selectWindow_ = addChild(GameSelectWindow::createTask(gameField_->getGameSystem()));
	selectWindow_->setPosition(kuto::Vector2(0.f, 160.f));
	selectWindow_->setSize(kuto::Vector2(320.f, 80.f));
	selectWindow_->setMessageAlign(GameMessageWindow::kAlignLeft);
	selectWindow_->pauseUpdate(true);

	nameInputMenu_ = addChild(GameNameInputMenu::createTask(gameField_->getGameSystem()));
	nameInputMenu_->pauseUpdate(true);

	shopMenu_ = addChild(GameShopMenu::createTask(gameField_->getGameSystem()));
	shopMenu_->pauseUpdate(true);

	addCommand<CODE_OPERATE_SWITCH>();
	addCommand<CODE_OPERATE_VAR>();
	addCommand<CODE_OPERATE_ITEM>();
	addCommand<CODE_OPERATE_MONEY>();
	addCommand<CODE_OPERATE_TIMER>();
	addCommand<CODE_GOTO_LABEL>();
	addCommand<CODE_PARTY_CHANGE>();
	addCommand<CODE_LOCATE_MOVE>();
	addCommand<CODE_LOCATE_SAVE>();
	addCommand<CODE_LOCATE_LOAD>();
	addCommand<CODE_TXT_SHOW>();
	addCommand<CODE_TXT_OPTION>();
	addCommand<CODE_TXT_FACE>();
	addCommand<CODE_BTL_GO_START>();
	addCommand<CODE_BTL_GO_WIN>();
	addCommand<CODE_BTL_GO_ESCAPE>();
	addCommand<CODE_BTL_GO_LOSE>();
	// comFuncMap_[CODE_BTL_GO_END] = &GameEventManager::command<CODE_IF_END>;
	addCommand<CODE_IF_START>();
	addCommand<CODE_IF_ELSE>();
	// addCommand<CODE_IF_END>();
	addCommand<CODE_SELECT_START>();
	addCommand<CODE_SELECT_CASE>();
	// comFuncMap_[CODE_SELECT_END] = &GameEventManager::command<CODE_IF_END>;
	addCommand<CODE_GAMEOVER>();
	addCommand<CODE_TITLE>();
	addCommand<CODE_EVENT_BREAK>();
	addCommand<CODE_EVENT_CLEAR>();
	addCommand<CODE_LOOP_START>();
	addCommand<CODE_LOOP_BREAK>();
	addCommand<CODE_LOOP_END>();
	addCommand<CODE_WAIT>();
	addCommand<CODE_PICT_SHOW>();
	addCommand<CODE_PICT_MOVE>();
	addCommand<CODE_PICT_CLEAR>();
	addCommand<CODE_SYSTEM_SCREEN>();
	addCommand<CODE_SCREEN_CLEAR>();
	addCommand<CODE_SCREEN_SHOW>();
	addCommand<CODE_SCREEN_SCROLL>();
	addCommand<CODE_CHARA_TRANS>();
	addCommand<CODE_PARTY_REFRESH>();
	addCommand<CODE_PARTY_EXP>();
	addCommand<CODE_PARTY_LV>();
	addCommand<CODE_PARTY_POWER>();
	addCommand<CODE_PARTY_SKILL>();
	addCommand<CODE_EVENT_GOSUB>();
	addCommand<CODE_CHARA_MOVE>();
	addCommand<CODE_MOVEALL_START>();
	addCommand<CODE_MOVEALL_CANCEL>();
	addCommand<CODE_NAME_INPUT>();
	addCommand<CODE_EVENT_SWAP>();
	addCommand<CODE_EVENT_LOCATE>();
	addCommand<CODE_PARTY_NAME>();
	addCommand<CODE_PARTY_TITLE>();
	addCommand<CODE_PARTY_WALK>();
	addCommand<CODE_PARTY_FACE>();
	addCommand<CODE_MM_BGM_PLAY>();
	addCommand<CODE_MM_SOUND>();
	addCommand<CODE_MM_BGM_SAVE>();
	addCommand<CODE_MM_BGM_LOAD>();
	addCommand<CODE_OPERATE_KEY>();
	addCommand<CODE_PANORAMA>();
	addCommand<CODE_INN>();
	addCommand<CODE_INN_IF_START>();
	addCommand<CODE_INN_IF_ELSE>();
	// comFuncMap_[CODE_INN_IF_END] = &GameEventManager::command<CODE_IF_END>;
	addCommand<CODE_SHOP>();
	comFuncMap_[CODE_SHOP_IF_START] = &GameEventManager::command<CODE_INN_IF_START>;
	comFuncMap_[CODE_SHOP_IF_ELSE] = &GameEventManager::command<CODE_INN_IF_ELSE>;
	// comFuncMap_[CODE_SHOP_IF_END] = &GameEventManager::command<CODE_IF_END>;
	addCommand<CODE_SCREEN_COLOR>();
	addCommand<CODE_BTLANIME>();
	addCommand<CODE_PARTY_SOUBI>();

	addCommand<CODE_TELEPORT>();
	addCommand<CODE_TELEPORT_PERM>();
	addCommand<CODE_ESCAPE>();
	addCommand<CODE_ESCAPE_PERM>();
	addCommand<CODE_MENU_PERM>();
	addCommand<CODE_SAVE_PERM>();

	addCommand<CODE_BLOCK_END>();

	addCommandWait<CODE_LOCATE_MOVE>();
	comWaitFuncMap_[CODE_LOCATE_LOAD] = &GameEventManager::commandWait<CODE_LOCATE_MOVE>;
	addCommandWait<CODE_TXT_SHOW>();
	addCommandWait<CODE_BTL_GO_START>();
	addCommandWait<CODE_SELECT_START>();
	addCommandWait<CODE_WAIT>();
	addCommandWait<CODE_PICT_MOVE>();
	addCommandWait<CODE_SCREEN_SCROLL>();
	comWaitFuncMap_[CODE_PARTY_EXP] = &GameEventManager::commandWait<CODE_TXT_SHOW>;
	comWaitFuncMap_[CODE_PARTY_LV] = &GameEventManager::commandWait<CODE_TXT_SHOW>;
	addCommandWait<CODE_NAME_INPUT>();
	addCommandWait<CODE_OPERATE_KEY>();
	addCommandWait<CODE_INN>();
	addCommandWait<CODE_SHOP>();
	addCommandWait<CODE_SCREEN_COLOR>();
	addCommandWait<CODE_BTLANIME>();

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
		shopMenu_->pauseUpdate(false);
		shopMenu_->freeze(true);

		bgm_->play();		// temp
		return true;
	}
	return false;
}

void GameEventManager::preMapChange()
{
	for (uint i = 0; i < pictures_.size(); i++) {
		if (pictures_[i]) {
			pictures_[i]->release();
			pictures_[i] = NULL;
		}
	}
	rpg2k::model::MapUnit& rpgLmu = gameField_->getGameSystem().getLMU();
	Array2D& mapEvent = rpgLmu.event();
	for (Array2D::Iterator it = mapEvent.begin(); it != mapEvent.end(); ++it) {
		if (eventPageInfos_[it.first()].npc) {
			eventPageInfos_[it.first()].npc->release();
			eventPageInfos_[it.first()].npc = NULL;
		}
		if (eventPageInfos_[it.first()].mapChip) {
			eventPageInfos_[it.first()].mapChip->release();
			eventPageInfos_[it.first()].mapChip = NULL;
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
	const rpg2k::model::DataBase& rpgLdb = gameField_->getGameSystem().getLDB();
	const rpg2k::model::MapUnit& rpgLmu = gameField_->getGameSystem().getLMU();
	eventPageInfos_.deallocate();
	eventPageInfos_.allocate(rpgLmu.event().rend().first() + 1 + rpgLdb.commonEvent().rend().first() + 1);
}

void GameEventManager::postMapChange()
{
	initEventPageInfos();
	updateEventAppear();
}

void GameEventManager::update()
{
	if (timer_.enable && timer_.count) timer_.count--;

	updateEventAppear();
	if (waitEventInfo_.enable)
		updateWaitEvent();
	else
		updateEvent();
	if (!waitEventInfo_.enable) {
		kuto::VirtualPad* virtualPad = kuto::VirtualPad::instance();
		bool pressMenu = virtualPad->press(kuto::VirtualPad::KEY_B);
		pressMenu = pressMenu && !gameField_->getPlayerLeader()->isMoving();
		// TODO: cannot open menu flag
		if (pressMenu) gameField_->startSystemMenu();
		else updateEncount();
	}
}

/*
std::string GameEventManager::getEncountBattleMap(const rpg2k::model::MapTree::MapInfo& mapInfo, int terrainId)
{
	if (mapInfo.m_BattleMapType == 0) {
		const rpg2k::model::MapTree::MapInfo& parentInfo = gameField_->getGameSystem().getLMT().m_saMapInfo[mapInfo.m_ParentMapID];
		return getEncountBattleMap(parentInfo, terrainId);
	} else if (mapInfo.m_BattleMapType == 1) {
		const rpg2k::model::DataBase& ldb = gameField_->getGameSystem().getLDB();
		return ldb.saTerrain[terrainId].battleGraphic;
	} else {
		return mapInfo.m_BattleMapName;
	}
}
 */

void GameEventManager::updateEncount()
{
	if (gameField_->getGame()->getConfig().noEncount)
		return;

/*
	GamePlayer* player = gameField_->getPlayerLeader();
	if (player->getMoveResult() == GameChara::kMoveResultDone) {
		encountStep_++;
		const rpg2k::model::DataBase& ldb = gameField_->getGameSystem().getLDB();
		const rpg2k::model::MapTree::MapInfo& mapInfo = gameField_->getGameSystem().getLMT().m_saMapInfo[gameField_->getMap()->getMapId()];
		float encountRatio = mapInfo.m_EnemyEncounter > 0? (float)(encountStep_) / ((float)mapInfo.m_EnemyEncounter * 2.f + 1.f) : -1.f;
		if (encountStep_ > 0 && encountRatio >= kuto::random(1.f) && mapInfo.m_enemyGroup.GetSize() > 0) {
			int terrainId = gameField_->getMap()->getTerrainId(player->getPosition().x, player->getPosition().y);
			std::string terrain = getEncountBattleMap(mapInfo, terrainId);
			std::vector<int> enableEnemyIds;
			for (uint i = 0; i < mapInfo.m_enemyGroup.GetSize(); i++) {
				int enemyGroupId = mapInfo.m_enemyGroup[i].enemyGroupID;
				if (mapInfo.m_BattleMapType != 2 ||
				 (terrainId > 0 && ((int)ldb.enemyGroup[enemyGroupId].appearTerrain.size() <= terrainId ||
				 ldb.enemyGroup[enemyGroupId].appearTerrain[terrainId]))) {
					enableEnemyIds.push_back(enemyGroupId);
				}
			}
			if (!enableEnemyIds.empty()) {
				gameField_->startBattle(terrain, enableEnemyIds[kuto::random((int)enableEnemyIds.size())], false, true, true);
				encountStep_ = -1;
			}
		}
	}
 */
}

/*
bool GameEventManager::isEventConditionOk(const rpg2k::structure::InstructionCondition& condition)
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	bool appear = true;
	if (condition.nFlag & rpg2k::structure::InstructionCondition::kFlagSwitch1) {
		if (!system.getLSD().getFlag(condition.nSw1))
			appear = false;
	}
	if (condition.nFlag & rpg2k::structure::InstructionCondition::kFlagSwitch2) {
		if (!system.getLSD().getFlag(condition.nSw2))
			appear = false;
	}
	if (condition.nFlag & rpg2k::structure::InstructionCondition::kFlagVar) {
		if (system.getLSD().getVar(condition.nVarNum) < condition.nVarOver)
			appear = false;
	}
	if (condition.nFlag & rpg2k::structure::InstructionCondition::kFlagItem) {
		if (system.getLSD().getItemNum(condition.nItem) == 0)
			appear = false;
	}
	if (condition.nFlag & rpg2k::structure::InstructionCondition::kFlagPlayer) {
		if (!gameField_->getPlayerFromId(condition.nChara))
			appear = false;
	}
	if (condition.nFlag & rpg2k::structure::InstructionCondition::kFlagTimer) {
		if (!timer_.enable || ((timer_.count + 59) / 60) > condition.nTimer)
			appear = false;
	}
	return appear;
}
 */

void GameEventManager::updateEventAppear()
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	rpg2k::model::MapUnit& rpgLmu = system.getLMU();
	rpg2k::model::SaveData& lsd = system.getLSD();

	for (uint i = 1; i < rpgLmu.event().rend().first(); i++) {
		Array1D& mapEvent = rpgLmu.event()[i];
		if (eventPageInfos_[i].cleared)
			continue;
		int pageIndex = 0;
		for (int iPage = mapEvent[5].getArray2D().rend().first() - 1; iPage >= 1; iPage--) {
			Array1D& eventPage = mapEvent[5].getArray2D()[iPage];
			bool appear = lsd.validPageMap(eventPage[2]);
			if (appear) {
				pageIndex = iPage;
				break;
			}
		}
		// change page
		if (eventPageInfos_[i].index != pageIndex) {
			if (eventPageInfos_[i].mapChip != NULL) {
				if (mapEvent[5].getArray2D()[eventPageInfos_[i].index][34].get<int>() == rpg2k::EventPriority::CHAR)
					gameField_->getCollision()->removeEventObject(eventPageInfos_[i].mapChip);
				eventPageInfos_[i].mapChip->release();
				eventPageInfos_[i].mapChip = NULL;
			}
			if (pageIndex > 0) {
				Array1D& eventPage = mapEvent[5].getArray2D()[pageIndex];
				kuto::u32 npcCrc = kuto::crc32(eventPage[21].get_string().toSystem());
				if (eventPageInfos_[i].npc != NULL && eventPageInfos_[i].npcCrc != npcCrc) {
					gameField_->getCollision()->removeChara(eventPageInfos_[i].npc);
					eventPageInfos_[i].npc->release();
					eventPageInfos_[i].npc = NULL;
				}
				if (!eventPage[21].get_string().empty()) {
					if (eventPageInfos_[i].npc) {
						eventPageInfos_[i].npc->setEventPage(eventPage);
						eventPageInfos_[i].npc->setDirection((rpg2k::EventDir::Type)eventPage[22].get<int>());
					} else {
						GameNpc* npc = addChild(kuto::TaskCreatorParam2<GameNpc, GameField*, rpg2k::structure::Array1D const&>::createTask(gameField_, eventPage));
						npc->setPosition(kuto::Point2(mapEvent[2].get<int>(), mapEvent[3].get<int>()));
						npc->loadWalkTexture(eventPage[21].get_string().toSystem(), eventPage[22].get<int>());
						npc->setDirection((rpg2k::EventDir::Type)eventPage[23].get<int>());

						gameField_->getCollision()->addChara(npc);
						eventPageInfos_[i].npc = npc;
					}
				} else {
					eventPageInfos_[i].mapChip = addChild(GameEventMapChip::createTask(system.getLDB(), gameField_->getMap()));
					eventPageInfos_[i].mapChip->setPosition(kuto::Point2(mapEvent[2].get<int>(), mapEvent[3].get<int>()));
					eventPageInfos_[i].mapChip->setPriority((rpg2k::EventPriority::Type)eventPage[34].get<int>());
					eventPageInfos_[i].mapChip->setPartsIndex(eventPage[23].get<int>());
					if (eventPage[34].get<int>() == rpg2k::EventPriority::CHAR)
						gameField_->getCollision()->addEventObject(eventPageInfos_[i].mapChip);
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
				eventPageInfos_[i].x = mapEvent[2].get<int>();
				eventPageInfos_[i].y = mapEvent[3].get<int>();
			}
		}
	}

	for (Array2D::Iterator it = system.getLDB().commonEvent().begin(); it != system.getLDB().commonEvent().end(); ++it) {
		int pageInfoIndex = it.first() + rpgLmu.event().rbegin().first();
		Array1D& commonEvent = it.second();
		int pageIndex = 0;
		if (
			(commonEvent[11].get<int>() != rpg2k::EventStart::CALLED) &&
			( !commonEvent[12].get<bool>() || ( commonEvent[12].get<bool>() && lsd.getFlag(commonEvent[13]) ) )
		) {
			pageIndex = 1;
		}
		eventPageInfos_[pageInfoIndex].index = pageIndex;
	}
}

void GameEventManager::updateEvent()
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	rpg2k::model::MapUnit& rpgLmu = system.getLMU();

	kuto::VirtualPad* virtualPad = kuto::VirtualPad::instance();
	bool pressOk = virtualPad->press(kuto::VirtualPad::KEY_A);
	GamePlayer* player = gameField_->getPlayerLeader();
	const kuto::Point2& playerPos = player->getPosition();
	rpg2k::EventDir::Type playerDir = player->getDirection();
	kuto::Point2 playerFrontPos = playerPos;
	switch (playerDir) {
	case rpg2k::EventDir::LEFT: 	playerFrontPos.x--; break;
	case rpg2k::EventDir::RIGHT: 	playerFrontPos.x++; break;
	case rpg2k::EventDir::UP: 	playerFrontPos.y--; break;
	case rpg2k::EventDir::DOWN: 	playerFrontPos.y++; break;
	}

	for (Array2D::Iterator it = rpgLmu.event().begin(); it != rpgLmu.event().end(); ++it) {
		Array1D& mapEvent = it.second();
		if (eventPageInfos_[it.first()].cleared)
			continue;
		currentEventIndex_ = it.first();
		if (eventPageInfos_[it.first()].index > 0) {
			Array1D& eventPage = mapEvent[5].getArray2D()[eventPageInfos_[it.first()].index];
			bool isStart = false;
			switch (eventPage[33].get<int>()) {
			case rpg2k::EventStart::KEY_ENTER:
				if (!waitEventInfo_.enable && pressOk) {
					if (eventPage[34].get<int>() == rpg2k::EventPriority::CHAR) {
						isStart = (playerFrontPos.x - eventPageInfos_[it.first()].x == 0 && playerFrontPos.y - eventPageInfos_[it.first()].y == 0);
						if (!isStart && gameField_->getMap()->isCounter(playerFrontPos.x, playerFrontPos.y)) {
							kuto::Point2 playerFrontFrontPos = playerFrontPos;
							switch (playerDir) {
							case rpg2k::EventDir::LEFT: 	playerFrontFrontPos.x--; break;
							case rpg2k::EventDir::RIGHT: 	playerFrontFrontPos.x++; break;
							case rpg2k::EventDir::UP: 	playerFrontFrontPos.y--; break;
							case rpg2k::EventDir::DOWN: 	playerFrontFrontPos.y++; break;
							}
							isStart = (playerFrontFrontPos.x - eventPageInfos_[it.first()].x == 0 && playerFrontFrontPos.y - eventPageInfos_[it.first()].y == 0);
						}
					} else {
						isStart = (playerPos.x - eventPageInfos_[it.first()].x == 0 && playerPos.y - eventPageInfos_[it.first()].y == 0);
					}
				}
				break;
			case rpg2k::EventStart::PARTY_TOUCH:
				if (!waitEventInfo_.enable) {
					if (eventPage[34].get<int>() == rpg2k::EventPriority::CHAR) {
						isStart = (playerFrontPos.x - eventPageInfos_[it.first()].x == 0 && playerFrontPos.y - eventPageInfos_[it.first()].y == 0);
						isStart = isStart && player->getMoveResult() == GameChara::kMoveResultCollied;
					} else {
						isStart = playerPos.x - eventPageInfos_[it.first()].x == 0 && playerPos.y - eventPageInfos_[it.first()].y == 0;
						isStart = isStart && player->getMoveResult() == GameChara::kMoveResultDone;
					}
					isStart = isStart && !player->isEnableRoute();
				}
				break;
			case rpg2k::EventStart::EVENT_TOUCH:
				if (!waitEventInfo_.enable && eventPageInfos_[it.first()].npc) {
					if (eventPage[34].get<int>() == rpg2k::EventPriority::CHAR) {
						rpg2k::EventDir::Type npcDir = eventPageInfos_[it.first()].npc->getDirection();
						isStart =
							(playerPos.x - eventPageInfos_[it.first()].x == 1 && playerPos.y - eventPageInfos_[it.first()].y == 0 && npcDir == rpg2k::EventDir::RIGHT)
						||  (playerPos.x - eventPageInfos_[it.first()].x == -1 && playerPos.y - eventPageInfos_[it.first()].y == 0 && npcDir == rpg2k::EventDir::LEFT)
						||  (playerPos.x - eventPageInfos_[it.first()].x == 0 && playerPos.y - eventPageInfos_[it.first()].y == 1 && npcDir == rpg2k::EventDir::DOWN)
						||  (playerPos.x - eventPageInfos_[it.first()].x == 0 && playerPos.y - eventPageInfos_[it.first()].y == -1 && npcDir == rpg2k::EventDir::UP);
						isStart = isStart && eventPageInfos_[it.first()].npc->getMoveResult() == GameChara::kMoveResultCollied;
					} else {
						isStart = playerPos.x - eventPageInfos_[it.first()].x == 0 && playerPos.y - eventPageInfos_[it.first()].y == 0;
						isStart = isStart && eventPageInfos_[it.first()].npc->getMoveResult() == GameChara::kMoveResultDone;
					}
					isStart = isStart && !player->isEnableRoute();
				}
				break;
			case rpg2k::EventStart::AUTO:
				if (!waitEventInfo_.enable) {
					isStart = true;
				}
				break;
			case rpg2k::EventStart::PARALLEL:
				//if (!nextWaitEventInfo_.enable)
					isStart = true;
				break;
			}
			if (!isStart)
				continue;

			startDecideButton_ = eventPage[33].get<int>() == rpg2k::EventStart::KEY_ENTER;
			executeChildCommands_ = true;
			conditionStack_.clear();
			loopStack_.clear();
			routeSetChara_ = NULL;
			backupWaitInfoEnable_ = waitEventInfo_.enable;
			waitEventInfo_.enable = false;
			executeCommands(eventPage[52], 0);
			if (backupWaitInfoEnable_)
				waitEventInfo_.enable = true;
		}
	}
	for (Array2D::Iterator it = system.getLDB().commonEvent().begin(); it != system.getLDB().commonEvent().end(); ++it) {
		currentEventIndex_ = it.first() + rpgLmu.event().rbegin().first() + 1;
		if (eventPageInfos_[currentEventIndex_].index > 0) {
			Array1D& eventPage = it.second();
			bool isStart = false;
			switch (eventPage[11].get<int>()) {
			case rpg2k::EventStart::AUTO:
				if (!waitEventInfo_.enable) {
					isStart = true;
				}
				break;
			case rpg2k::EventStart::PARALLEL:
				//if (!nextWaitEventInfo_.enable)
					isStart = true;
				break;
			}
			if (!isStart)
				continue;

			startDecideButton_ = eventPage[11].get<int>() == rpg2k::EventStart::KEY_ENTER;
			executeChildCommands_ = true;
			conditionStack_.clear();
			loopStack_.clear();
			routeSetChara_ = NULL;
			backupWaitInfoEnable_ = waitEventInfo_.enable;
			waitEventInfo_.enable = false;
			executeCommands(eventPage[22], 0);
			if (backupWaitInfoEnable_)
				waitEventInfo_.enable = true;
		}
	}
}

void GameEventManager::executeCommands(const rpg2k::structure::Event& eventPage, int start)
{
	currentEventPage_ = &eventPage;
	// create label
	std::memset(labels_.get(), 0, labels_.size() * sizeof(int));
	for (currentCommandIndex_ = 0; currentCommandIndex_ < eventPage.size(); currentCommandIndex_++) {
		const rpg2k::structure::Instruction& com = eventPage[currentCommandIndex_];
		if (com.code() == CODE_LABEL) {
			labels_[com.at(0) - 1] = currentCommandIndex_;
		}
	}
	// execute command
	for (currentCommandIndex_ = start; currentCommandIndex_ < eventPage.size(); currentCommandIndex_++) {
		const rpg2k::structure::Instruction& com = eventPage[currentCommandIndex_];
		if (!conditionStack_.empty() && uint(conditionStack_.top().nest) < com.nest() && !executeChildCommands_) {
			continue;
		}
		//executeChildCommands_ = false;

		ComFuncMap::iterator it;
		if ( ( it = comFuncMap_.find(com.code()) ) != comFuncMap_.end()) {
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
						const kuto::Point2& playerPos = player->getPosition();
						const kuto::Point2& npcPos = eventPageInfos_[currentEventIndex_].npc->getPosition();
						rpg2k::EventDir::Type dir =
							(playerPos.x < npcPos.x) ? rpg2k::EventDir::LEFT  :
							(playerPos.x > npcPos.x) ? rpg2k::EventDir::RIGHT :
							(playerPos.y > npcPos.y) ? rpg2k::EventDir::DOWN  :
							(playerPos.y < npcPos.y) ? rpg2k::EventDir::UP    :
							rpg2k::EventDir::DOWN;
						eventPageInfos_[currentEventIndex_].npc->startTalking(dir);
					}
				}
				break;
			}
		} else {
			// rpg2k::debug::Tracer::printInstruction(com, std::cout) << std::endl;
		}
		rpg2k::debug::Tracer::printInstruction(com, std::cout) << std::endl;
	}
	if (!waitEventInfo_.enable && !backupWaitInfoEnable_) {
		gameMessageWindow_->setFaceTexture("", 0, false, false);	// reset face?
	}
}

void GameEventManager::updateWaitEvent()
{
	const rpg2k::structure::Event& eventPage = *waitEventInfo_.page;
	int comStartPos = waitEventInfo_.pos;
	executeChildCommands_ = waitEventInfo_.executeChildCommands;
	conditionStack_ = waitEventInfo_.conditionStack;
	loopStack_ = waitEventInfo_.loopStack;
	routeSetChara_ = NULL;
	// execute command
	const rpg2k::structure::Instruction& com = eventPage[comStartPos];
	kuto_assert(comWaitFuncMap_.find(com.code()) != comWaitFuncMap_.end());
	(this->*(comWaitFuncMap_.find(com.code())->second))(com);

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
	case 10002:		// 小型船 TODO
		chara = gameField_->getPlayerLeader();
		break;
	case 10003:		// 大型船 TODO
		chara = gameField_->getPlayerLeader();
		break;
	case 10004:		// 飛行船 TODO
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
		// TODO
	}
}

void GameEventManager::openGameSelectWindow()
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
		// TODO
	}
}

void GameEventManager::addLevelUpMessage(const GameCharaStatus& status, int oldLevel)
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	rpg2k::model::DataBase const& ldb = system.getLDB();
	int charID = status.getCharaId();

	std::ostringstream oss( system.name(charID).toSystem() );
	oss << "は" << ldb.vocabulary(123).toSystem() << system.level(charID) << ldb.vocabulary(36).toSystem();
	gameMessageWindow_->addLine(oss.str());

	std::vector< uint16_t >& skillList = system.getLSD().skill(charID);
	Array2D const& charSkill = ldb.character()[charID][63];
	int level = 1;
	for(Array2D::Iterator it = charSkill.begin(); it != charSkill.end(); ++it) {
		if( it.second().exists(1) ) level = it.second()[1];
		int skillID = it.second()[2].get<int>();
		if(level > oldLevel && level <= status.getLevel() &&
			std::find( skillList.begin(), skillList.end(), skillID ) == skillList.end()
		) {
			gameMessageWindow_->addLine(system.getLDB().skill()[skillID][1].get_string().toSystem() + ldb.vocabulary(37).toSystem());
		}
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

void GameEventManager::draw()
{
	if (timer_.draw) {
		// TODO
	}
}
