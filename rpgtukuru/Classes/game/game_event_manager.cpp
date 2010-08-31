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

#include "game_event_manager.h"
#include "game_event_command_decl.h"
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
#include "game_shop_menu.h"
#include "game_event_map_chip.h"
#include "game_skill_anime.h"

using rpg2k::structure::Array1D;
using rpg2k::structure::Array2D;


GameEventManager::GameEventManager(kuto::Task* parent, GameField* field)
: kuto::Task(parent)
, gameField_(field)
, currentEventPage_(NULL), executeChildCommands_(true), encountStep_(0), routeSetChara_(NULL)
, bgm_(NULL), skillAnime_(NULL)
{
	bgm_ = GameBgm::createTask(this, "");	// temp
	// const rpg2k::model::MapUnit& rpgLmu = gameField_->getMap()->getRpgLmu();
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
	shopMenu_ = GameShopMenu::createTask(this, gameField_->getGameSystem());
	shopMenu_->pauseUpdate(true);

	comFuncMap_[CODE_OPERATE_SWITCH] = &GameEventManager::command<CODE_OPERATE_SWITCH>;
	comFuncMap_[CODE_OPERATE_VAR] = &GameEventManager::command<CODE_OPERATE_VAR>;
	comFuncMap_[CODE_OPERATE_ITEM] = &GameEventManager::command<CODE_OPERATE_ITEM>;
	comFuncMap_[CODE_OPERATE_MONEY] = &GameEventManager::command<CODE_OPERATE_MONEY>;
	comFuncMap_[CODE_OPERATE_TIMER] = &GameEventManager::command<CODE_OPERATE_TIMER>;
	comFuncMap_[CODE_GOTO_LABEL] = &GameEventManager::command<CODE_GOTO_LABEL>;
	comFuncMap_[CODE_PARTY_CHANGE] = &GameEventManager::command<CODE_PARTY_CHANGE>;
	comFuncMap_[CODE_LOCATE_MOVE] = &GameEventManager::command<CODE_LOCATE_MOVE>;
	comFuncMap_[CODE_LOCATE_SAVE] = &GameEventManager::command<CODE_LOCATE_SAVE>;
	comFuncMap_[CODE_LOCATE_LOAD] = &GameEventManager::command<CODE_LOCATE_LOAD>;
	comFuncMap_[CODE_TXT_SHOW] = &GameEventManager::command<CODE_TXT_SHOW>;
	comFuncMap_[CODE_TXT_OPTION] = &GameEventManager::command<CODE_TXT_OPTION>;
	comFuncMap_[CODE_TXT_FACE] = &GameEventManager::command<CODE_TXT_FACE>;
	comFuncMap_[CODE_BTL_GO_START] = &GameEventManager::command<CODE_BTL_GO_START>;
	comFuncMap_[CODE_BTL_GO_WIN] = &GameEventManager::command<CODE_BTL_GO_WIN>;
	comFuncMap_[CODE_BTL_GO_ESCAPE] = &GameEventManager::command<CODE_BTL_GO_ESCAPE>;
	comFuncMap_[CODE_BTL_GO_LOSE] = &GameEventManager::command<CODE_BTL_GO_LOSE>;
	comFuncMap_[CODE_BTL_GO_END] = &GameEventManager::command<CODE_IF_END>;
	comFuncMap_[CODE_IF_START] = &GameEventManager::command<CODE_IF_START>;
	comFuncMap_[CODE_IF_ELSE] = &GameEventManager::command<CODE_IF_ELSE>;
	comFuncMap_[CODE_IF_END] = &GameEventManager::command<CODE_IF_END>;
	comFuncMap_[CODE_SELECT_START] = &GameEventManager::command<CODE_SELECT_START>;
	comFuncMap_[CODE_SELECT_CASE] = &GameEventManager::command<CODE_SELECT_CASE>;
	comFuncMap_[CODE_SELECT_END] = &GameEventManager::command<CODE_IF_END>;
	comFuncMap_[CODE_GAMEOVER] = &GameEventManager::command<CODE_GAMEOVER>;
	comFuncMap_[CODE_TITLE] = &GameEventManager::command<CODE_TITLE>;
	comFuncMap_[CODE_EVENT_BREAK] = &GameEventManager::command<CODE_EVENT_BREAK>;
	comFuncMap_[CODE_EVENT_CLEAR] = &GameEventManager::command<CODE_EVENT_CLEAR>;
	comFuncMap_[CODE_LOOP_START] = &GameEventManager::command<CODE_LOOP_START>;
	comFuncMap_[CODE_LOOP_BREAK] = &GameEventManager::command<CODE_LOOP_BREAK>;
	comFuncMap_[CODE_LOOP_END] = &GameEventManager::command<CODE_LOOP_END>;
	comFuncMap_[CODE_WAIT] = &GameEventManager::command<CODE_WAIT>;
	comFuncMap_[CODE_PICT_SHOW] = &GameEventManager::command<CODE_PICT_SHOW>;
	comFuncMap_[CODE_PICT_MOVE] = &GameEventManager::command<CODE_PICT_MOVE>;
	comFuncMap_[CODE_PICT_CLEAR] = &GameEventManager::command<CODE_PICT_CLEAR>;
	comFuncMap_[CODE_SYSTEM_SCREEN] = &GameEventManager::command<CODE_SYSTEM_SCREEN>;
	comFuncMap_[CODE_SCREEN_CLEAR] = &GameEventManager::command<CODE_SCREEN_CLEAR>;
	comFuncMap_[CODE_SCREEN_SHOW] = &GameEventManager::command<CODE_SCREEN_SHOW>;
	comFuncMap_[CODE_SCREEN_SCROLL] = &GameEventManager::command<CODE_SCREEN_SCROLL>;
	comFuncMap_[CODE_CHARA_TRANS] = &GameEventManager::command<CODE_CHARA_TRANS>;
	comFuncMap_[CODE_PARTY_REFRESH] = &GameEventManager::command<CODE_PARTY_REFRESH>;
	comFuncMap_[CODE_PARTY_EXP] = &GameEventManager::command<CODE_PARTY_EXP>;
	comFuncMap_[CODE_PARTY_LV] = &GameEventManager::command<CODE_PARTY_LV>;
	comFuncMap_[CODE_PARTY_POWER] = &GameEventManager::command<CODE_PARTY_POWER>;
	comFuncMap_[CODE_PARTY_SKILL] = &GameEventManager::command<CODE_PARTY_SKILL>;
	comFuncMap_[CODE_EVENT_GOSUB] = &GameEventManager::command<CODE_EVENT_GOSUB>;
	comFuncMap_[CODE_CHARA_MOVE] = &GameEventManager::command<CODE_CHARA_MOVE>;
	comFuncMap_[CODE_MOVEALL_START] = &GameEventManager::command<CODE_MOVEALL_START>;
	comFuncMap_[CODE_MOVEALL_CANSEL] = &GameEventManager::command<CODE_MOVEALL_CANSEL>;
	comFuncMap_[CODE_NAME_INPUT] = &GameEventManager::command<CODE_NAME_INPUT>;
	comFuncMap_[CODE_PARTY_NAME] = &GameEventManager::command<CODE_PARTY_NAME>;
	comFuncMap_[CODE_PARTY_TITLE] = &GameEventManager::command<CODE_PARTY_TITLE>;
	comFuncMap_[CODE_PARTY_WALK] = &GameEventManager::command<CODE_PARTY_WALK>;
	comFuncMap_[CODE_PARTY_FACE] = &GameEventManager::command<CODE_PARTY_FACE>;
	comFuncMap_[CODE_MM_BGM_PLAY] = &GameEventManager::command<CODE_MM_BGM_PLAY>;
	comFuncMap_[CODE_OPERATE_KEY] = &GameEventManager::command<CODE_OPERATE_KEY>;
	comFuncMap_[CODE_PANORAMA] = &GameEventManager::command<CODE_PANORAMA>;
	comFuncMap_[CODE_INN] = &GameEventManager::command<CODE_INN>;
	comFuncMap_[CODE_INN_IF_START] = &GameEventManager::command<CODE_INN_IF_START>;
	comFuncMap_[CODE_INN_IF_ELSE] = &GameEventManager::command<CODE_INN_IF_ELSE>;
	comFuncMap_[CODE_INN_IF_END] = &GameEventManager::command<CODE_IF_END>;
	comFuncMap_[CODE_SHOP] = &GameEventManager::command<CODE_SHOP>;
	comFuncMap_[CODE_SHOP_IF_START] = &GameEventManager::command<CODE_INN_IF_START>;
	comFuncMap_[CODE_SHOP_IF_ELSE] = &GameEventManager::command<CODE_INN_IF_ELSE>;
	comFuncMap_[CODE_SHOP_IF_END] = &GameEventManager::command<CODE_IF_END>;
	comFuncMap_[CODE_MM_SOUND] = &GameEventManager::command<CODE_MM_SOUND>;
	comFuncMap_[CODE_SCREEN_COLOR] = &GameEventManager::command<CODE_SCREEN_COLOR>;
	comFuncMap_[CODE_BTLANIME] = &GameEventManager::command<CODE_BTLANIME>;
	comFuncMap_[CODE_PARTY_SOUBI] = &GameEventManager::command<CODE_PARTY_SOUBI>;

	// TODO: CODE_IF_END -> CODE_BLOCK_END

	comWaitFuncMap_[CODE_LOCATE_MOVE] = &GameEventManager::commandWait<CODE_LOCATE_MOVE>;
	comWaitFuncMap_[CODE_LOCATE_LOAD] = &GameEventManager::commandWait<CODE_LOCATE_MOVE>;
	comWaitFuncMap_[CODE_TXT_SHOW] = &GameEventManager::commandWait<CODE_TXT_SHOW>;
	comWaitFuncMap_[CODE_BTL_GO_START] = &GameEventManager::commandWait<CODE_BTL_GO_START>;
	comWaitFuncMap_[CODE_SELECT_START] = &GameEventManager::commandWait<CODE_SELECT_START>;
	comWaitFuncMap_[CODE_WAIT] = &GameEventManager::commandWait<CODE_WAIT>;
	comWaitFuncMap_[CODE_PICT_MOVE] = &GameEventManager::commandWait<CODE_PICT_MOVE>;
	comWaitFuncMap_[CODE_SCREEN_SCROLL] = &GameEventManager::commandWait<CODE_SCREEN_SCROLL>;
	comWaitFuncMap_[CODE_PARTY_EXP] = &GameEventManager::commandWait<CODE_TXT_SHOW>;
	comWaitFuncMap_[CODE_PARTY_LV] = &GameEventManager::commandWait<CODE_TXT_SHOW>;
	comWaitFuncMap_[CODE_NAME_INPUT] = &GameEventManager::commandWait<CODE_NAME_INPUT>;
	comWaitFuncMap_[CODE_OPERATE_KEY] = &GameEventManager::commandWait<CODE_OPERATE_KEY>;
	comWaitFuncMap_[CODE_INN] = &GameEventManager::commandWait<CODE_INN>;
	comWaitFuncMap_[CODE_SHOP] = &GameEventManager::commandWait<CODE_SHOP>;
	comWaitFuncMap_[CODE_SCREEN_COLOR] = &GameEventManager::commandWait<CODE_SCREEN_COLOR>;
	comWaitFuncMap_[CODE_BTLANIME] = &GameEventManager::commandWait<CODE_BTLANIME>;

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
		// TODO cannot open menu flag
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
/*
	if (gameField_->getGameSystem().getConfig().noEncount)
		return;

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
						GameNpc* npc = GameNpc::createTask(this, gameField_, eventPage);
						npc->setPosition(kuto::Point2(mapEvent[2].get<int>(), mapEvent[3].get<int>()));
						npc->loadWalkTexture(eventPage[21].get_string().toSystem(), eventPage[22].get<int>());
						npc->setDirection((rpg2k::EventDir::Type)eventPage[23].get<int>());

						gameField_->getCollision()->addChara(npc);
						eventPageInfos_[i].npc = npc;
					}
				} else {
					eventPageInfos_[i].mapChip = GameEventMapChip::createTask(this, system.getLDB(), gameField_->getMap());
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
		currentEventIndex_ = it.first() + rpgLmu.event().rbegin().first();
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
	for (currentCommandIndex_ = 0; currentCommandIndex_ < (int)eventPage.size(); currentCommandIndex_++) {
		const rpg2k::structure::Instruction& com = eventPage[currentCommandIndex_];
		if (com.code() == CODE_LABEL) {
			labels_[com.at(0) - 1] = currentCommandIndex_;
		}
	}
	// execute command
	for (currentCommandIndex_ = start; currentCommandIndex_ < (int)eventPage.size(); currentCommandIndex_++) {
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
			kuto_assert(false);
			//kuto_printf("unknown command %x¥n", com.code());
		}
		//kuto_printf("event command %x¥n", com.code());
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

PP_protoType(CODE_OPERATE_SWITCH)
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	int rangeA = 0;
	int rangeB = 0;
	switch (com.at(0)) {
	case 0:		// 0:[単独] S[A]
		rangeA = com.at(1);
		rangeB = rangeA;
		break;
	case 1:		// 1:[一括] S[A]〜S[B]
		rangeA = com.at(1);
		rangeB = com.at(2);
		break;
	case 2:		// 2:[変数] S[V[A]]
		rangeA = system.getLSD().getVar(com.at(1));
		rangeB = rangeA;
		break;
	}
	for (int iSwitch = rangeA; iSwitch <= rangeB; iSwitch++) {
		if (com.at(3) == 0) {			// 0:ONにする
			system.getLSD().setFlag(iSwitch, true);
		} else if (com.at(3) == 1) {	// 1:OFFにする
			system.getLSD().setFlag(iSwitch, false);
		} else if (com.at(3) == 2) {	// 2:ON/OFFを逆転
			system.getLSD().setFlag(iSwitch, !system.getLSD().getFlag(iSwitch));
		}
	}
}

PP_protoType(CODE_OPERATE_VAR)
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	rpg2k::model::SaveData& lsd = system.getLSD();
	int rangeA = 0;
	int rangeB = 0;
	switch (com.at(0)) {
	case 0:		// 0:[単独] S[A]
		rangeA = com.at(1);
		rangeB = rangeA;
		break;
	case 1:		// 1:[一括] S[A]〜S[B]
		rangeA = com.at(1);
		rangeB = com.at(2);
		break;
	case 2:		// 2:[変数] S[V[A]]
		rangeA = system.getLSD().getVar(com.at(1));
		rangeB = rangeA;
		break;
	}
	int value = 0;
	switch (com.at(4)) {
	case 0:		// [定数] Cの値を直接適用
		value = com.at(5);
		break;
	case 1:		// [変数] V[C]
		value = system.getLSD().getVar(com.at(5));
		break;
	case 2:		// [変数(参照)] V[V[C]]
		value = system.getLSD().getVar(system.getLSD().getVar(com.at(5)));
		break;
	case 3:		// [乱数] C〜Dの範囲の乱数
		value = kuto::random(com.at(6) + 1) + com.at(5);
		break;
	case 4:		// [アイテム]
		if (com.at(6) == 0)	// 所持数
			value = system.getLSD().getItemNum(com.at(5));
		else {							// 装備数
			value = 0;
			for (uint i = 0; i < gameField_->getPlayers().size(); i++) {
				const GameCharaStatus& status = gameField_->getPlayers()[i]->getStatus();
				if (status.getEquip()[rpg2k::Equip::WEAPON] == com.at(5) ||
				status.getEquip()[rpg2k::Equip::SHIELD] == com.at(5) ||
				status.getEquip()[rpg2k::Equip::ARMOR] == com.at(5) ||
				status.getEquip()[rpg2k::Equip::HELMET] == com.at(5) ||
				status.getEquip()[rpg2k::Equip::OTHER] == com.at(5))
					value++;
			}
		}
		break;
	case 5:		// [主人公]
		{
			//const GameCharaStatus& status = gameField_->getGameSystem().getPlayerStatus(com.at(5));
			int charID = com.at(5);
			switch (com.at(6)) {
			case 0:		value = system.level(charID);		break;		// レベル
			case 1:		value = system.exp(charID);		break;		// 経験値
			case 2:		value = system.hp(charID);		break;		// HP
			case 3:		value = system.mp(charID);		break;		// MP
			case 4:		value = system.param(charID, rpg2k::Param::HP);		break;		// 最大HP
			case 5:		value = system.param(charID, rpg2k::Param::MP);		break;		// 最大MP
			case 6:		value = system.param(charID, rpg2k::Param::ATTACK);		break;		// 攻撃力
			case 7:		value = system.param(charID, rpg2k::Param::GAURD);		break;		// 防御力
			case 8:		value = system.param(charID, rpg2k::Param::MIND);		break;		// 精神力
			case 9:		value = system.param(charID, rpg2k::Param::SPEED);		break;		// 敏捷力
			case 10: case 11: case 12: case 13: case 14: {
				 std::vector< uint16_t > equip = lsd.character()[charID][61].getBinary();
				 value = equip[com.at(6)-10];
			} break;
			}
		}
		break;
	case 6:		// [キャラ]
		{
			GameChara* chara = getCharaFromEventId(com.at(5));
			switch (com.at(6)) {
			case 0:		// マップID
				switch (com.at(5)) {
				case 10002:		// 小型船 TODO
				case 10003:		// 大型船 TODO
				case 10004:		// 飛行船 TODO
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
					case rpg2k::EventDir::UP:		value = 8;	break;
					case rpg2k::EventDir::RIGHT:	value = 6;	break;
					case rpg2k::EventDir::DOWN:		value = 2;	break;
					case rpg2k::EventDir::LEFT:		value = 4;	break;
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
		switch( com.at(5) ) {
		case 0: value = system.getLSD().getMoney(); break; // 所持金
		case 1: value = (timer_.count + 59) / 60; break; // タイマー1の残り秒数
		case 2: value = system.getLSD().memberNum(); break; // パーティ人数
		case 3: value = system.getLSD()[101].getArray1D()[131]; break; // セーブ回数
		case 4: value = system.getLSD()[109].getArray1D()[32]; break; // 戦闘回数
		case 5: value = system.getLSD()[109].getArray1D()[34]; break; // 勝利回数
		case 6: value = system.getLSD()[109].getArray1D()[33]; break; // 敗北回数
		case 7: value = system.getLSD()[109].getArray1D()[35]; break; // 逃走回数
		case 8: value = 0; break; // TODO // MIDIの演奏位置(Tick)
		}
		break;
	case 8:		// [敵キャラ]
		// TODO
		break;
	}
	for (int iSwitch = rangeA; iSwitch <= rangeB; iSwitch++) {
		if (com.at(3) == 0) {			// 0:代入
			system.getLSD().setVar(iSwitch, value);
		} else if (com.at(3) == 1) {	// 1:加算
			system.getLSD().setVar(iSwitch, system.getLSD().getVar(iSwitch) + value);
		} else if (com.at(3) == 2) {	// 2:減算
			system.getLSD().setVar(iSwitch, system.getLSD().getVar(iSwitch) - value);
		} else if (com.at(3) == 3) {	// 3:乗算
			system.getLSD().setVar(iSwitch, system.getLSD().getVar(iSwitch) * value);
		} else if (com.at(3) == 4) {	// 4:除算
			system.getLSD().setVar(iSwitch, system.getLSD().getVar(iSwitch) / value);
		} else if (com.at(3) == 5) {	// 5:剰余
			system.getLSD().setVar(iSwitch, system.getLSD().getVar(iSwitch) % value);
		}
	}
}

PP_protoType(CODE_OPERATE_ITEM)
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	int itemId = com.at(1) == 0? com.at(2) : system.getLSD().getVar(com.at(2));
	int num = com.at(3) == 0? com.at(4) : system.getLSD().getVar(com.at(4));
	system.getLSD().addItemNum(itemId, com.at(0) == 0? num : -num);
}

PP_protoType(CODE_OPERATE_MONEY)
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	int num = com.at(1) == 0? com.at(2) : system.getLSD().getVar(com.at(2));
	system.getLSD().addMoney(com.at(0) == 0? num : -num);
}

PP_protoType(CODE_OPERATE_TIMER)
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	switch (com.at(0)) {
	case 0:		// 値の設定
		timer_.count = com.at(1) == 0? com.at(2) : system.getLSD().getVar(com.at(2));
		timer_.count *= 60;		// sec to frame
		break;
	case 1:		// 作動開始
		timer_.enable = true;
		timer_.draw = (com.at(3) == 1);
		timer_.enableBattle = (com.at(4) == 1);
		break;
	case 2:		// 作動停止
		timer_.enable = false;
		break;
	}
}

PP_protoType(CODE_GOTO_LABEL)
{
	currentCommandIndex_ = labels_[com.at(0) - 1];		// 戻り先で+1されるのでちょうどLabelの次になる
}

PP_protoType(CODE_PARTY_CHANGE)
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	int playerId = com.at(1) == 0? com.at(2) : system.getLSD().getVar(com.at(2));
	if (com.at(0) == 0) {		// メンバーを加える
		gameField_->addPlayer(playerId);
	} else {							// メンバーを外す
		gameField_->removePlayer(playerId);
	}
}

PP_protoType(CODE_LOCATE_MOVE)
{
	int dir = com.getArgNum() >= 4? com.at(3) : 0;
	gameField_->changeMap(com.at(0), com.at(1), com.at(2), dir);
	waitEventInfo_.enable = true;
}

PP_protoTypeWait(CODE_LOCATE_MOVE)
{
	waitEventInfo_.enable = false;
}

PP_protoType(CODE_LOCATE_SAVE)
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	system.getLSD().setVar(com.at(0), system.getLSD().eventState(rpg2k::EV_ID_PARTY).mapID());
	system.getLSD().setVar(com.at(1), system.getLSD().eventState(rpg2k::EV_ID_PARTY).x());
	system.getLSD().setVar(com.at(2), system.getLSD().eventState(rpg2k::EV_ID_PARTY).y());
}

PP_protoType(CODE_LOCATE_LOAD)
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	gameField_->changeMap(system.getLSD().getVar(com.at(0)), system.getLSD().getVar(com.at(1)), system.getLSD().getVar(com.at(2)), 0);
	waitEventInfo_.enable = true;
}

PP_protoType(CODE_TXT_SHOW)
{
	openGameMassageWindow();
	gameMessageWindow_->addLine( com.getString().toSystem() );
	for (uint i = currentCommandIndex_ + 1; i < currentEventPage_->instNum(); i++) {
		const rpg2k::structure::Instruction& comNext = (*currentEventPage_)[i];
		if (comNext.code() == CODE_TXT_SHOW_ADD) {
			gameMessageWindow_->addLine(comNext.getString().toSystem());
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
		// TODO
	}
}

PP_protoTypeWait(CODE_TXT_SHOW)
{
	if (gameMessageWindow_->closed()) {
		waitEventInfo_.enable = false;
		gameMessageWindow_->freeze(true);
	}
}

PP_protoType(CODE_TXT_OPTION)
{
	messageWindowSetting_.showFrame = !com.at(0);
	messageWindowSetting_.pos = (MessageWindowSetting::PosType)com.at(1);
	messageWindowSetting_.autoMove = com.at(2);
	messageWindowSetting_.enableOtherEvent = com.at(3);
}

PP_protoType(CODE_TXT_FACE)
{
	gameMessageWindow_->setFaceTexture(com.getString().toSystem(), com.at(0), (bool)com.at(1), (bool)com.at(2));
	selectWindow_->setFaceTexture(com.getString().toSystem(), com.at(0), (bool)com.at(1), (bool)com.at(2));
}

PP_protoType(CODE_BTL_GO_START)
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	std::string terrain;
	if (com.at(2) == 0) {
		int terrainId = gameField_->getMap()->getTerrainId(eventPageInfos_[currentEventIndex_].x, eventPageInfos_[currentEventIndex_].y);
		terrain = system.getLDB().terrain()[terrainId][4].get_string().toSystem();
	} else {
		terrain = com.getString().toSystem();
	}
	int enemyId = com.at(0) == 0? com.at(1) : system.getLSD().getVar(com.at(1));
	gameField_->startBattle(terrain, enemyId, (bool)com.at(5), com.at(3) != 0, com.at(4) == 0);
	waitEventInfo_.enable = true;
}

PP_protoTypeWait(CODE_BTL_GO_START)
{
	waitEventInfo_.enable = false;
	int result = gameField_->getBattleResult();
	if (com.at(3) == 1 && result == GameBattle::kResultEscape) {
		command<CODE_EVENT_BREAK>(com);		// Escape -> Break
	} else if (com.at(3) == 2 || com.at(4) == 1) {
		conditionStack_.push(ConditionInfo(com.nest(), result));
	}
}

PP_protoType(CODE_BTL_GO_WIN)
{
	executeChildCommands_ = conditionStack_.top().value == GameBattle::kResultWin;
}

PP_protoType(CODE_BTL_GO_ESCAPE)
{
	executeChildCommands_ = conditionStack_.top().value == GameBattle::kResultEscape;
}

PP_protoType(CODE_BTL_GO_LOSE)
{
	executeChildCommands_ = conditionStack_.top().value == GameBattle::kResultLose;
}

PP_protoType(CODE_IF_START)
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	int condValue = 0;
	switch (com.at(0)) {
	case 0:		// 0:スイッチ
		condValue = system.getLSD().getFlag(com.at(1)) == (com.at(2) == 0? true : false);
		break;
	case 1:		// 1:変数
		{
			int value = com.at(2) == 0? com.at(3) : system.getLSD().getVar(com.at(3));
			switch (com.at(4)) {
			case 0:		// と同値
				condValue = (system.getLSD().getVar(com.at(1)) == value);
				break;
			case 1:		// 	以上
				condValue = (system.getLSD().getVar(com.at(1)) >= value);
				break;
			case 2:		// 	以下
				condValue = (system.getLSD().getVar(com.at(1)) <= value);
				break;
			case 3:		// より大きい
				condValue = (system.getLSD().getVar(com.at(1)) > value);
				break;
			case 4:		// より小さい
				condValue = (system.getLSD().getVar(com.at(1)) < value);
				break;
			case 5:		// 以外
				condValue = (system.getLSD().getVar(com.at(1)) != value);
				break;
			}
		}
		break;
	case 2:		// 2:タイマー1
		switch (com.at(2)) {
		case 0:		// 以上
			condValue = (timer_.enable? timer_.count >= com.at(1) * 60 : false);
			break;
		case 1:		// 以下
			condValue = (timer_.enable? timer_.count <= com.at(1) * 60 : false);
			break;
		}
		break;
	case 3:		// 3:所持金
		switch (com.at(2)) {
		case 0:		// 以上
			condValue = (system.getLSD().getMoney() >= com.at(1));
			break;
		case 1:		// 以下
			condValue = (system.getLSD().getMoney() <= com.at(1));
			break;
		}
		break;
	case 4:		// 4:アイテム
		switch (com.at(2)) {
		case 0:		// 持っている
			condValue = (system.getLSD().getItemNum(com.at(1)) > 0);
			break;
		case 1:		// 持っていない
			condValue = (system.getLSD().getItemNum(com.at(1)) == 0);
			break;
		}
		break;
	case 5:		// 5:主人公
		switch (com.at(2)) {
		case 0:		// パーティにいる
			condValue = gameField_->getPlayerFromId(com.at(1)) != NULL;
			break;
		case 1:		// 主人公の名前が文字列引数と等しい
			condValue = com.getString() == system.name(com.at(1));
			break;
		case 2:		// レベルがCの値以上
			condValue = system.level(com.at(1)) >= com.at(3);
			break;
		case 3:		// HPがCの値以上
			condValue = system.hp(com.at(1)) >= com.at(3);
			break;
		case 4:		// 特殊技能IDがCの値の特殊技能を使用できる
			// condValue = gameField_->getGameSystem().getPlayerStatus(com.at(1)).isLearnedSkill(com.at(3));
			break;
		case 5:		// アイテムIDがCの値のアイテムを装備している
			/*
			condValue = gameField_->getGameSystem().getPlayerStatus(com.at(1)).getEquip()[rpg2k::Equip::WEAPON] == com.at(3) ||
				gameField_->getGameSystem().getPlayerStatus(com.at(1)).getEquip()[rpg2k::Equip::SHIELD] == com.at(3) ||
				gameField_->getGameSystem().getPlayerStatus(com.at(1)).getEquip()[rpg2k::Equip::ARMOR] == com.at(3) ||
				gameField_->getGameSystem().getPlayerStatus(com.at(1)).getEquip()[rpg2k::Equip::HELMET] == com.at(3) ||
				gameField_->getGameSystem().getPlayerStatus(com.at(1)).getEquip()[rpg2k::Equip::OTHER] == com.at(3);
			 */
			break;
		case 6:		// 状態IDがCの状態になっている
			// condValue = gameField_->getGameSystem().getPlayerStatus(com.at(1)).getBadConditionIndex(com.at(3)) >= 0;
			break;
		}
		break;
	case 6:		// 6:キャラの向き
		{
			GameChara* chara = getCharaFromEventId(com.at(1));
			if (chara) {
				condValue = chara->getDirection() == com.at(2);
			} else
				condValue = false;
		}
		break;
	case 7:		// 7:乗り物
		condValue = (false);	// TODO
		break;
	case 8:		// 8:決定キーでこのイベントを開始した
		condValue = startDecideButton_;
		break;
	case 9:		// 9:演奏中のBGMが一周した
		condValue = bgm_->isLooped();	// TODO
		break;
	}
	conditionStack_.push(ConditionInfo(com.nest(), condValue));
	executeChildCommands_ = conditionStack_.top().value == true;
}

PP_protoType(CODE_IF_ELSE)
{
	executeChildCommands_ = conditionStack_.top().value == false;
}

PP_protoType(CODE_IF_END)
{
	conditionStack_.pop();
	executeChildCommands_ = true;
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

PP_protoType(CODE_SELECT_START)
{
	openGameSelectWindow();
	std::string::size_type oldPos = 0;
	for (int i = 0; i < 4; i++) {
		std::string::size_type pos = com.getString().find('/', oldPos);
		if (pos == std::string::npos) {
			selectWindow_->addLine(com.getString().substr(oldPos));
			break;
		} else {
			selectWindow_->addLine(com.getString().substr(oldPos, pos - oldPos));
			oldPos = pos + 1;
		}
	}
	selectWindow_->setEnableCancel(com.at(0) != 0);

	waitEventInfo_.enable = true;
}

PP_protoTypeWait(CODE_SELECT_START)
{
	if (selectWindow_->closed()) {
		waitEventInfo_.enable = false;
		selectWindow_->freeze(true);
		int selectIndex = selectWindow_->cursor();
		if (selectWindow_->canceled()) {
			selectIndex = com.at(0) - 1;
		}
		int value = selectIndex < (int)selectWindow_->getMessageSize()? kuto::crc32(selectWindow_->getMessage(selectIndex).str) : 0;
		conditionStack_.push(ConditionInfo(com.nest(), value));
	}
}

PP_protoType(CODE_SELECT_CASE)
{
	executeChildCommands_ = (uint)conditionStack_.top().value == kuto::crc32(com.getString().toSystem());
}

PP_protoType(CODE_GAMEOVER)
{
	gameField_->gameOver();
	waitEventInfo_.enable = true;
}

PP_protoType(CODE_TITLE)
{
	gameField_->returnTitle();
	waitEventInfo_.enable = true;
}

PP_protoType(CODE_EVENT_BREAK)
{
	currentCommandIndex_ = 100000;		// 100000行もないだろ
}

PP_protoType(CODE_EVENT_CLEAR)
{
	eventPageInfos_[currentEventIndex_].cleared = true;
	if (eventPageInfos_[currentEventIndex_].npc) {
		eventPageInfos_[currentEventIndex_].npc->freeze(true);
	}
}

PP_protoType(CODE_LOOP_START)
{
	LoopInfo info;
	info.startIndex = currentCommandIndex_;
	info.conditionSize = conditionStack_.size();
	for (uint i = currentCommandIndex_ + 1; i < currentEventPage_->instNum(); i++) {
		const rpg2k::structure::Instruction& comNext = (*currentEventPage_)[i];
		if (com.nest() != comNext.nest()) {
			continue;
		}
		if (comNext.code() == CODE_LOOP_END) {
			info.endIndex = i;
			break;
		}
	}
	loopStack_.push(info);
}

PP_protoType(CODE_LOOP_BREAK)
{
	while (loopStack_.top().conditionSize < (int)conditionStack_.size()) {
		conditionStack_.pop();
	}
	currentCommandIndex_ = loopStack_.top().endIndex;		// 戻り先で+1されるのでちょうどLoopStartの次になる
	loopStack_.pop();
}

PP_protoType(CODE_LOOP_END)
{
	currentCommandIndex_ = loopStack_.top().startIndex;		// 戻り先で+1されるのでちょうどLoopStartの次になる
}

PP_protoType(CODE_WAIT)
{
	waitEventInfo_.enable = true;
}

PP_protoTypeWait(CODE_WAIT)
{
	waitEventInfo_.count++;
	float nowSec = (float)waitEventInfo_.count / 60.f;
	float waitSec = (float)com.at(0) / 10.f;
	if (nowSec >= waitSec) {
		waitEventInfo_.enable = false;
	}
}

static void setPictureInfo(GameEventPicture::Info& info, const rpg2k::structure::Instruction& com, GameField* field)
{
	rpg2k::model::Project& system = field->getGameSystem();
	info.map = field->getMap();
	info.position.x = com.at(1) == 0? com.at(2) : system.getLSD().getVar(com.at(2));
	info.position.y = com.at(1) == 0? com.at(3) : system.getLSD().getVar(com.at(3));
	info.scroll = (bool)com.at(4);
	info.scale = (float)com.at(5) / 100.f;
	info.color.a = 1.f - (float)com.at(6) / 100.f;
	info.useAlpha = (bool)com.at(7);
	info.color.r = (float)com.at(8) / 100.f;
	info.color.g = (float)com.at(9) / 100.f;
	info.color.b = (float)com.at(10) / 100.f;
	info.saturation = (float)com.at(11) / 100.f;
	info.effect = (GameEventPicture::EffectType)com.at(12);
	info.effectSpeed = com.at(13);
}

PP_protoType(CODE_PICT_SHOW)
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	int picIndex = com.at(0) - 1;
	if (pictures_[picIndex]) {
		pictures_[picIndex]->release();
		pictures_[picIndex] = NULL;
	}
	GameEventPicture::Info info;
	//setPictureInfo(info, com, gameField_);

	pictures_[picIndex] = GameEventPicture::createTask(this, std::string( system.gameDir() ).append("/Picture/").append( com.getString().toSystem() ), info);
	pictures_[picIndex]->setPriority(1.f + (float)picIndex * -0.0001f);
}

PP_protoType(CODE_PICT_MOVE)
{
	int picIndex = com.at(0) - 1;
	if (!pictures_[picIndex]) {
		return;
	}
	GameEventPicture::Info info;
	setPictureInfo(info, com, gameField_);

	pictures_[picIndex]->move(info, com.at(14) * 60 / 10);
	waitEventInfo_.enable = (bool)com.at(15);
}

PP_protoTypeWait(CODE_PICT_MOVE)
{
	int picIndex = com.at(0) - 1;
	if (!pictures_[picIndex]->isMoving()) {
		waitEventInfo_.enable = false;
	}
}

PP_protoType(CODE_PICT_CLEAR)
{
	int picIndex = com.at(0) - 1;
	if (pictures_[picIndex]) {
		pictures_[picIndex]->release();
		pictures_[picIndex] = NULL;
	}
}

PP_protoType(CODE_SYSTEM_SCREEN)
{
	gameField_->setFadeInfo(com.at(0), com.at(1));
}

PP_protoType(CODE_SCREEN_CLEAR)
{
	gameField_->fadeOut(com.at(0));
}

PP_protoType(CODE_SCREEN_SHOW)
{
	gameField_->fadeIn(com.at(0));
}

PP_protoType(CODE_SCREEN_SCROLL)
{
	GameMap* map = gameField_->getMap();
	if (com.at(0) < 2)
		map->setEnableScroll((bool)com.at(0));
	else {
		if (com.at(0) == 2) {
			int x = 0;
			int y = 0;
			if (com.at(1) == 0)	// 上
				y = -com.at(2);
			else if (com.at(1) == 1)	// 右
				x = com.at(2);
			else if (com.at(1) == 2)	// 下
				y = com.at(2);
			else if (com.at(1) == 3)	// 左
				x = -com.at(2);
			map->scroll(x, y, (float)(1 << com.at(3)));
		} else
			map->scrollBack((float)(1 << com.at(3)));
		if (com.at(4) == 1);
			waitEventInfo_.enable = true;
	}
}

PP_protoTypeWait(CODE_SCREEN_SCROLL)
{
	if (!gameField_->getMap()->isScrolling())
		waitEventInfo_.enable = false;
}

PP_protoType(CODE_CHARA_TRANS)
{
	gameField_->getPlayerLeader()->setVisible((bool)com.at(0));
}

void GameEventManager::addLevelUpMessage(const GameCharaStatus& status, int oldLevel)
{
/*
	rpg2k::model::Project& system = gameField_->getGameSystem();
	const rpg2k::model::DataBase& ldb = system.getLDB();
	const GamePlayerInfo& player = system.getPlayerInfo(status.getCharaId());
	char temp[256];
	sprintf(temp, "%sは%s%d%s", player.name.c_str(), ldb.vocabulary(123).toSystem().c_str(),
		status.getLevel(), ldb.vocabulary(36).toSystem().c_str());
	gameMessageWindow_->addLine(temp);
	for (uint iLearn = 1; iLearn < player.baseInfo->learnSkill.size(); iLearn++) {
		const rpg2k::model::DataBase::LearnSkill& learnSkill = player.baseInfo->learnSkill[iLearn];
		if (learnSkill.level > oldLevel && learnSkill.level <= status.getLevel()) {
			gameMessageWindow_->addLine(system.getLDB().skill[learnSkill.skill][1].get_string().toSystem() + ldb.vocabulary(37).toSystem());
		}
	}
 */
}

PP_protoType(CODE_PARTY_REFRESH)
{
	rpg2k::model::Project& proj = gameField_->getGameSystem();
	rpg2k::model::SaveData& lsd = proj.getLSD();
	Array2D& charDatas = lsd.character();
	std::vector< uint > charIDs;

	switch(com[0]) {
		case 0: {
			std::vector< uint16_t >& member = lsd.member();
			for( uint i = 0; i < member.size(); i++ ) charIDs.push_back(member[i]);
		} break;
		case 1: charIDs.push_back(com[1]); break;
		case 2: charIDs.push_back( lsd.getVar(com[1]) ); break;
	}

	for( std::vector< uint >::const_iterator it = charIDs.begin(); it != charIDs.end(); it++ ) {
		charDatas[*it][71] = proj.param(*it, rpg2k::Param::HP);
		charDatas[*it][72] = proj.param(*it, rpg2k::Param::MP);
	}
}

PP_protoType(CODE_PARTY_EXP)
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	kuto::StaticVector<GameCharaStatus*, 4> statusList;
	switch (com.at(0)) {
	case 0:		// 0:パーティーメンバー全員
		for (uint i = 0; i < gameField_->getPlayers().size(); i++) {
			statusList.push_back(&gameField_->getPlayers()[i]->getStatus());
		}
		break;
	case 1:		// 1:[固定] 主人公IDがAの主人公
		// statusList.push_back(&system.getPlayerStatus(com.at(1)));
		break;
	case 2:		// 2:[変数] 主人公IDがV[A]の主人公
		// statusList.push_back(&system.getPlayerStatus(system.getLSD().getVar(com.at(1))));
		break;
	}
	int exp = com.at(3) == 0? com.at(4) : system.getLSD().getVar(com.at(4));
	kuto::StaticVector<std::pair<GameCharaStatus*, int>, 4> levelUpList;
	for (uint i = 0; i < statusList.size(); i++) {
		int oldLevel = statusList[i]->getLevel();
		statusList[i]->addExp(com.at(2) == 0? exp : -exp);
		if (statusList[i]->getLevel() > oldLevel) {
			levelUpList.push_back(std::make_pair(statusList[i], oldLevel));
		}
	}
	if (com.at(5) == 1 && !levelUpList.empty()) {
		openGameMassageWindow();
		for (uint i = 0; i < levelUpList.size(); i++) {
			addLevelUpMessage(*levelUpList[i].first, levelUpList[i].second);
		}
		waitEventInfo_.enable = true;
	}
}

PP_protoType(CODE_PARTY_LV)
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	kuto::StaticVector<GameCharaStatus*, 4> statusList;
	switch (com.at(0)) {
	case 0:		// 0:パーティーメンバー全員
		for (uint i = 0; i < gameField_->getPlayers().size(); i++) {
			statusList.push_back(&gameField_->getPlayers()[i]->getStatus());
		}
		break;
	case 1:		// 1:[固定] 主人公IDがAの主人公
		// statusList.push_back(&system.getPlayerStatus(com.at(1)));
		break;
	case 2:		// 2:[変数] 主人公IDがV[A]の主人公
		// statusList.push_back(&system.getPlayerStatus(system.getLSD().getVar(com.at(1))));
		break;
	}
	int level = com.at(3) == 0? com.at(4) : system.getLSD().getVar(com.at(4));
	kuto::StaticVector<std::pair<GameCharaStatus*, int>, 4> levelUpList;
	for (uint i = 0; i < statusList.size(); i++) {
		int oldLevel = statusList[i]->getLevel();
		statusList[i]->addLevel(com.at(2) == 0? level : -level);
		if (statusList[i]->getLevel() > oldLevel) {
			levelUpList.push_back(std::make_pair(statusList[i], oldLevel));
		}
	}
	if (com.at(5) == 1 && !levelUpList.empty()) {
		openGameMassageWindow();
		for (uint i = 0; i < levelUpList.size(); i++) {
			addLevelUpMessage(*levelUpList[i].first, levelUpList[i].second);
		}
		waitEventInfo_.enable = true;
	}
}

PP_protoType(CODE_PARTY_POWER)
{
	rpg2k::model::SaveData& lsd = gameField_->getGameSystem().getLSD();
	Array2D& charDatas = lsd.character();

	std::vector< uint16_t > target;
	switch(com[0]) {
		case 0: target = lsd.member(); break;
		case 1: target.push_back(com[1]); break;
		case 2: target.push_back( lsd.getVar(com[1]) ); break;
	}

	int index, max, min;
	switch(com[3]) {
		case rpg2k::Param::HP:
			max = rpg2k::CHAR_HP_MAX; min = rpg2k::PARAM_MIN;
			index = 33 + com[3];
			break;
		case rpg2k::Param::MP:
			max =   rpg2k::PARAM_MAX; min =    rpg2k::MP_MIN;
			index = 33 + com[3];
			break;
		case rpg2k::Param::ATTACK: case rpg2k::Param::GAURD: case rpg2k::Param::MIND: case rpg2k::Param::SPEED:
			max =   rpg2k::PARAM_MAX; min = rpg2k::PARAM_MIN;
			index = 41 + com[3] - rpg2k::Param::ATTACK;
			break;
	}

	int val = com[5];
	if(val == 0) val = 1;
	else switch(com[4]) {
		case 0: break;
		case 1: val = lsd.getVar(val); break;
	}
	switch(com[2]) {
		case 0: val =  val; break;
		case 1: val = -val; break;
	}

	for( std::vector< uint16_t >::const_iterator it = target.begin(); it != target.end(); it++ ) {
		charDatas[*it][index] = charDatas[*it][index].get<int>() + val;
	}
}

PP_protoType(CODE_PARTY_SKILL)
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	kuto::StaticVector<GameCharaStatus*, 4> statusList;
	switch (com.at(0)) {
	case 0:		// 0:パーティーメンバー全員
		for (uint i = 0; i < gameField_->getPlayers().size(); i++) {
			statusList.push_back(&gameField_->getPlayers()[i]->getStatus());
		}
		break;
	case 1:		// 1:[固定] 主人公IDがAの主人公
		// statusList.push_back(&system.getPlayerStatus(com.at(1)));
		break;
	case 2:		// 2:[変数] 主人公IDがV[A]の主人公
		// statusList.push_back(&system.getPlayerStatus(system.getLSD().getVar(com.at(1))));
		break;
	}
	int skillId = com.at(3) == 0? com.at(4) : system.getLSD().getVar(com.at(4));
	for (uint i = 0; i < statusList.size(); i++) {
		if (com.at(2) == 0)
			statusList[i]->learnSkill(skillId);
		else
			statusList[i]->forgetSkill(skillId);
	}
}

PP_protoType(CODE_EVENT_GOSUB)
{
	const rpg2k::model::MapUnit& rpgLmu = gameField_->getGameSystem().getLMU();
	rpg2k::model::Project& system = gameField_->getGameSystem();
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
	if (com.at(0) == 0) {
		// common event
		int eventId = com.at(1);
		currentEventIndex_ = eventId + rpgLmu.event().rbegin().first();
		executeCommands(system.getLDB().commonEvent()[eventId][22], 0);
	} else {
		// map event
		int eventId = (com.at(0) == 1)? com.at(1) : system.getLSD().getVar(com.at(1));
		int eventPage = (com.at(0) == 1)? com.at(2) : system.getLSD().getVar(com.at(2));
		if (eventId == 10005)
			eventId = currentEventIndex_;
		currentEventIndex_ = eventId;
		executeCommands(rpgLmu.event()[eventId][5].getArray2D()[eventPage][52], 0);
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

PP_protoType(CODE_CHARA_MOVE)
{
/*
	GameChara* chara = getCharaFromEventId(com.at(0));
	if (!chara)
		return;
	// int frequency = com.at(1);
	CRpgRoute route;
	route.repeat = com.at(2) == 1;
	route.ignore = com.at(3) == 1;
	for (uint i = 4; i < com.getArgNum(); i++) {
		route.commands.push_back(com.at(i));
		switch (com.at(i)) {
		case 32:	// スイッチON
		case 33:	// スイッチOFF
			route.extraIntParam.push_back(com.getExtraIntParam(i, 0));
			break;
		case 34:	// グラフィック変更
			route.extraStringParam.push_back(com.getExtraStringParam(i, 0));
			route.extraIntParam.push_back(com.getExtraIntParam(i, 0));
			break;
		case 35:	// 効果音の演奏
			route.extraStringParam.push_back(com.getExtraStringParam(i, 0));
			route.extraIntParam.push_back(com.getExtraIntParam(i, 0));
			route.extraIntParam.push_back(com.getExtraIntParam(i, 1));
			route.extraIntParam.push_back(com.getExtraIntParam(i, 2));
			break;
		}
	}
	chara->setRoute(route);
	routeSetChara_ = chara;
	routeSetChara_->startRoute();
 */
}

PP_protoType(CODE_MOVEALL_START)
{
	if (!routeSetChara_)
		return;
	routeSetChara_->startRoute();
}

PP_protoType(CODE_MOVEALL_CANSEL)
{
	if (!routeSetChara_)
		return;
	routeSetChara_->endRoute();
}

PP_protoType(CODE_NAME_INPUT)
{
	nameInputMenu_->freeze(false);
	nameInputMenu_->setPlayerInfo(com.at(0), (bool)com.at(1), (bool)com.at(2));
	waitEventInfo_.enable = true;
}

PP_protoTypeWait(CODE_NAME_INPUT)
{
	if (nameInputMenu_->closed()) {
		waitEventInfo_.enable = false;
		nameInputMenu_->freeze(true);
	}
}

PP_protoType(CODE_PARTY_NAME)
{
	gameField_->getGameSystem().setName( com.at(0), com.getString() );
}

PP_protoType(CODE_PARTY_TITLE)
{
	gameField_->getGameSystem().setTitle( com.at(0), com.getString() );
}

PP_protoType(CODE_PARTY_WALK)
{
	Array1D& charData = gameField_->getGameSystem().getLSD().character()[ com[0] ];

	charData[11] = com.getString();
	charData[12] = com[1];
	charData[13] = bool(com[2]);
}

PP_protoType(CODE_PARTY_FACE)
{
	Array1D& charData = gameField_->getGameSystem().getLSD().character()[ com[0] ];

	charData[21] = com.getString();
	charData[22] = com[1];
}

PP_protoType(CODE_MM_BGM_PLAY)
{
	// TODO
}

int getInputKeyValue(const rpg2k::structure::Instruction& com)
{
	kuto::VirtualPad* virtualPad = kuto::VirtualPad::instance();
	int key = 0;
	if (com.at(2) == 1) {
		if (virtualPad->press(kuto::VirtualPad::KEY_DOWN))
			key = 1;
		if (virtualPad->press(kuto::VirtualPad::KEY_LEFT))
			key = 2;
		if (virtualPad->press(kuto::VirtualPad::KEY_RIGHT))
			key = 3;
		if (virtualPad->press(kuto::VirtualPad::KEY_UP))
			key = 4;
	} else {
		if (com.at(5) == 1 && virtualPad->press(kuto::VirtualPad::KEY_X))
			key = 1;
		if (com.at(6) == 1 && virtualPad->press(kuto::VirtualPad::KEY_DOWN))
			key = 1;
		if (com.at(7) == 1 && virtualPad->press(kuto::VirtualPad::KEY_LEFT))
			key = 2;
		if (com.at(8) == 1 && virtualPad->press(kuto::VirtualPad::KEY_RIGHT))
			key = 3;
		if (com.at(9) == 1 && virtualPad->press(kuto::VirtualPad::KEY_UP))
			key = 4;
	}
	if (com.at(3) == 1 && virtualPad->press(kuto::VirtualPad::KEY_A))
		key = 5;
	if (com.at(4) == 1 && virtualPad->press(kuto::VirtualPad::KEY_B))
		key = 6;
	return key;
}

PP_protoType(CODE_OPERATE_KEY)
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	waitEventInfo_.enable = com.at(1);
	if (!waitEventInfo_.enable)
		system.getLSD().setVar(com.at(0), getInputKeyValue(com));
}

PP_protoTypeWait(CODE_OPERATE_KEY)
{
	int key = getInputKeyValue(com);
	if (key != 0) {
		waitEventInfo_.enable = false;
		rpg2k::model::Project& system = gameField_->getGameSystem();
		system.getLSD().setVar(com.at(0), key);
	}
}

PP_protoType(CODE_PANORAMA)
{
	Array1D& map = gameField_->getGameSystem().getLSD()[111];
	map[31] = true;
	map[32] = com.getString();
	map[33] = bool(com.at(0));
	map[34] = bool(com.at(1));
	map[35] = bool(com.at(2));
	map[36] = int(com.at(3));
	map[37] = bool(com.at(4));
	map[38] = int(com.at(5));
/*
	rpg2k::model::MapUnit::PanoramaInfo info;
	info.enable = true;
	info.name = com.getString();
	info.loopHorizontal = (com.at(0) == 1);
	info.loopVertical = (com.at(1) == 1);
	info.scrollHorizontal = (com.at(2) == 1);
	info.scrollSpeedHorizontal = com.at(3);
	info.scrollVertical = (com.at(4) == 1);
	info.scrollSpeedVertical = com.at(5);
	gameField_->getMap()->getRpgLmu().SetPanoramaInfo(info);
 */
}

PP_protoType(CODE_INN)
{
	openGameSelectWindow();
	const rpg2k::model::Project& system = gameField_->getGameSystem();
	const rpg2k::model::DataBase& ldb = system.getLDB();
	int base = 80 + com.at(0) * 5;
	std::string mes = ldb.vocabulary(base + 0);
	std::ostringstream oss;
	oss << com.at(1);
	mes += oss.str();
	mes += ldb.vocabulary(95).toSystem();
	mes += ldb.vocabulary(base + 1);
	selectWindow_->addLine(mes);
	selectWindow_->addLine(ldb.vocabulary(base + 2));
	selectWindow_->addLine(ldb.vocabulary(base + 3), system.getLSD().getMoney() >= com.at(1));
	selectWindow_->addLine(ldb.vocabulary(base + 4));
	selectWindow_->setCursorStart(2);
	selectWindow_->setEnableCancel(true);
	waitEventInfo_.enable = true;
}

PP_protoTypeWait(CODE_INN)
{
	if (selectWindow_->closed()) {
		waitEventInfo_.enable = false;
		selectWindow_->freeze(true);
		int selectIndex = selectWindow_->cursor();
		if (selectWindow_->canceled())
			selectIndex = 3;
		if (selectIndex == 2) {
			gameField_->getGameSystem().getLSD().addMoney(-com.at(1));
			for (uint i = 0; i < gameField_->getPlayers().size(); i++) {
				gameField_->getPlayers()[i]->getStatus().fullCure();
			}
		}
		if (com.at(2) == 1)
			conditionStack_.push(ConditionInfo(com.nest(), selectIndex == 2));
	}
}

PP_protoType(CODE_INN_IF_START)
{
	executeChildCommands_ = conditionStack_.top().value == true;
}

PP_protoType(CODE_INN_IF_ELSE)
{
	executeChildCommands_ = conditionStack_.top().value == false;
}

PP_protoType(CODE_SHOP)
{
	shopMenu_->freeze(false);
	int shopType = com.at(0);
	int mesType = com.at(1);
	std::vector<int> items;
	for (uint i = 4; i < com.getArgNum(); i++)
		items.push_back(com.at(i));
	shopMenu_->setShopData(shopType, mesType, items);
	waitEventInfo_.enable = true;
}

PP_protoTypeWait(CODE_SHOP)
{
	if (shopMenu_->closed()) {
		shopMenu_->freeze(true);
		waitEventInfo_.enable = false;
		if (com.at(2) == 1)
			conditionStack_.push(ConditionInfo(com.nest(), shopMenu_->buyOrSell()));
	}
}

PP_protoType(CODE_MM_SOUND)
{
	// TODO
}

PP_protoType(CODE_SCREEN_COLOR)
{
	// TODO
	waitEventInfo_.enable = (com.at(5) == 1);
}

PP_protoTypeWait(CODE_SCREEN_COLOR)
{
	waitEventInfo_.count++;
	float nowSec = (float)waitEventInfo_.count / 60.f;
	float waitSec = (float)com.at(4) / 10.f;
	if (nowSec >= waitSec) {
		waitEventInfo_.enable = false;
	}
}

PP_protoType(CODE_BTLANIME)
{
	GameSkillAnime* anime = GameSkillAnime::createTask(this, gameField_->getGameSystem(), com.at(0));
	int eventId = com.at(1);
	GameChara* chara = getCharaFromEventId(eventId);
	if (chara) {
		anime->setPlayPosition(kuto::Vector2(chara->getPosition().x * 16.f, chara->getPosition().y * 16.f));
	} else {
		anime->setPlayPosition(kuto::Vector2(eventPageInfos_[eventId].x * 16.f, eventPageInfos_[eventId].y * 16.f));
	}
	waitEventInfo_.enable = (com.at(2) == 1);
	if (waitEventInfo_.enable)
		skillAnime_ = anime;
	else
		anime->setDeleteFinished(true);
	anime->play();
}

PP_protoTypeWait(CODE_BTLANIME)
{
	if (skillAnime_->isFinished()) {
		skillAnime_->release();
		skillAnime_ = NULL;
		waitEventInfo_.enable = false;
	}
}

PP_protoType(CODE_PARTY_SOUBI)
{
	rpg2k::model::Project& proj = gameField_->getGameSystem();
	rpg2k::model::SaveData& lsd = proj.getLSD();

	std::vector<uint16_t> target;
	switch(com[0]) {
		case 0: target = lsd.member(); break;
		case 1: target.push_back(com[1]); break;
		case 2: target.push_back( lsd.getVar(com[1]) ); break;
	}

	switch(com[2]) {
	// equip
		case 0: {
			uint itemID;
			switch(com[4]) {
				case 0: itemID = (com[3] == 0) ? 1 : com[3]; break;
				case 1: itemID = lsd.getVar(com[3]); break;
			}

			for(std::vector<uint16_t>::const_iterator it = target.begin(); it != target.end(); it++) {
				if( !proj.equip(*it, itemID) ) break;
			}
		} break;
	// unequip
		case 1: {
			uint start, end;
			switch(com[3]) {
				case rpg2k::Equip::WEAPON: case rpg2k::Equip::SHIELD: case rpg2k::Equip::ARMOR: case rpg2k::Equip::HELMET: case rpg2k::Equip::OTHER:
					start = end = com[3]; break;
				case rpg2k::Equip::END: // all
					start = 0; end = rpg2k::Equip::END-1; break;
			}
			for(std::vector<uint16_t>::const_iterator it = target.begin(); it != target.end(); ++it) {
				proj.unequip( *it, static_cast< rpg2k::Equip::Type >(end) );
			}
		} break;
	}
}


void GameEventManager::draw()
{
	if (timer_.draw) {
		// TODO
	}
}
