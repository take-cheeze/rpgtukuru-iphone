/**
 * @file
 * @brief Game Field
 * @author project.kuto
 */

#include <kuto/kuto_file.h>
#include <kuto/kuto_stringstream.h>
#include "game_field.h"
#include "game_map.h"
#include "game_player.h"
#include "game_event_manager.h"
#include "game_collision.h"
#include "game_battle.h"
#include "game.h"
#include "game_fade_effect.h"
#include "game_save_data.h"
#include "game_system_menu.h"
#include "game_debug_menu.h"


GameField::GameField(Game* parent, GameSystem& gameSystem, int saveId)
: kuto::Task(parent)
, game_(parent), gameSystem_(gameSystem), gameBattle_(NULL), state_(kStateField)
{
	// int mapId;
	// GameChara::Point playerPos;
	int playerDir = GameChara::kDirDown;
	std::vector<int> playerIds;
/*
	if (saveId > 0) {
		std::ostringstream ss;
		initStringStream(ss);
		ss.str("");
		ss << kuto::Directory::getHomeDirectory() << "/Documents/" << kuto::File::getFileName( gameSystem_.getRpgLdb().getRootFolder() );
		string dir = ss.str();

		ss.str("");
		ss << gameSystem_.getRootFolder() << "/Save" std::setw(2) << saveId << "lsdi";
		// ss << dir << "/Save" std::setw(2) << saveId << "lsdi";
		GameSaveData* saveData = (GameSaveData*)kuto::File::readBytes( ss.str() );
		saveData->load(this);
		
		mapId = saveData->getLocation().getMapId();
		playerPos.x = saveData->getLocation().getX();
		playerPos.y = saveData->getLocation().getY();
		playerDir = saveData->getLocation().getDirection();
		for (int i = 0; i < saveData->getHeader().partyNum_; i++) {
			playerIds.push_back(saveData->getHeader().partyId_[i]);
		}
		
		kuto::File::freeBytes(saveData);
	} else {
		mapId = gameSystem_.getRpgLmt().m_PartyPosition.mapId;
		playerPos.x = gameSystem_.getRpgLmt().m_PartyPosition.x;
		playerPos.y = gameSystem_.getRpgLmt().m_PartyPosition.y;
		for (u32 i = 0; i < gameSystem_.getRpgLdb().system.startParty.size(); i++)
			playerIds.push_back(gameSystem_.getRpgLdb().system.startParty[i]);
	}
 */
	systemMenu_ = GameSystemMenu::createTask(this);
	gameCollision_ = GameCollision::createTask(this);
	
	gameMap_ = GameMap::createTask(this);
	// gameMap_->load(mapId, gameSystem_.getRpgLdb(), gameSystem_.getRootFolder().c_str());
	gameCollision_->setMap(gameMap_);
	
	dummyLeader_ = GamePlayer::createTask(this, 1, gameSystem_.getPlayerStatus(1));
	dummyLeader_->pauseUpdate(true);
	// dummyLeader_->setPosition(playerPos);
	dummyLeader_->setDirection((GameChara::DirType)playerDir);

	// gameEventManager_ = GameEventManager::createTask(this, this);
	fadeEffect_ = GameFadeEffect::createTask(this);
	fadeEffectScreen_ = GameFadeEffect::createTask(this);
	fadeInfos_[kFadePlaceMapHide] = GameFadeEffect::kTypeFade;
	fadeInfos_[kFadePlaceMapShow] = GameFadeEffect::kTypeFade;
	fadeInfos_[kFadePlaceBattleStartHide] = GameFadeEffect::kTypeStripeVertical;
	fadeInfos_[kFadePlaceBattleStartShow] = GameFadeEffect::kTypeFade;
	fadeInfos_[kFadePlaceBattleEndHide] = GameFadeEffect::kTypeFade;
	fadeInfos_[kFadePlaceBattleEndShow] = GameFadeEffect::kTypeFade;	

	for (u32 i = 0; i < playerIds.size(); i++)
		addPlayer(playerIds[i]);
	
	debugMenu_ = GameDebugMenu::createTask(this);
}

GameField::~GameField()
{
	if (debugMenu_)
		debugMenu_->release();		// parentで作成しているのでreleaseを呼ぶ必要がある
}

bool GameField::initialize()
{
	return isInitializedChildren();
}

void GameField::update()
{
	switch (state_) {
	case kStateField:
		break;
	case kStateBattleStart:
		if (fadeEffect_->getState() == GameFadeEffect::kStateFadeOutEnd) {
			state_ = kStateBattle;
			gameBattle_->freeze(false);
			fadeEffect_->start((GameFadeEffect::FadeType)fadeInfos_[kFadePlaceBattleStartShow], GameFadeEffect::kStateFadeIn);
			gameMap_->freeze(true);
			gameCollision_->freeze(true);
			gameEventManager_->freeze(true);
			if (!gamePlayers_.empty())
				gamePlayers_[0]->freeze(true);
		}
		break;
	case kStateBattle:
		if (gameBattle_->getState() == GameBattle::kStateEnd) {
			state_ = kStateBattleEnd;
			gameBattle_->pauseUpdate(true);
			fadeEffect_->start((GameFadeEffect::FadeType)fadeInfos_[kFadePlaceBattleEndHide], GameFadeEffect::kStateFadeOut);
		}
		break;
	case kStateBattleEnd:
		if (fadeEffect_->getState() == GameFadeEffect::kStateFadeOutEnd) {
			endBattle();
		}
		break;
	case kStateChangeMap:
		if (fadeEffect_->getState() == GameFadeEffect::kStateFadeOutEnd) {
			// gameEventManager_->preMapChange();
			
			gameMap_->release();
			gameCollision_->release();
			//gameEventManager_->release();
			
			gameCollision_ = GameCollision::createTask(this);
			gameMap_ = GameMap::createTask(this);
			// gameMap_->load(mapChangeInfo_.mapId, gameSystem_.getRpgLdb(), gameSystem_.getRootFolder().c_str());
			gameCollision_->setMap(gameMap_);
			//gameEventManager_ = GameEventManager::createTask(this, this);
			if (!gamePlayers_.empty()) {
				gamePlayers_[0]->pauseUpdate(false);
				gameCollision_->addChara(gamePlayers_[0]);
			}
			getPlayerLeader()->setPosition(GameChara::Point(mapChangeInfo_.x, mapChangeInfo_.y));
			if (mapChangeInfo_.dir > 0)
				getPlayerLeader()->setDirection((GameChara::DirType)(mapChangeInfo_.dir - 1));
			getPlayerLeader()->updateMapPosition();

			// gameEventManager_->postMapChange();
			gameEventManager_->pauseUpdate(false);
			fadeEffect_->start((GameFadeEffect::FadeType)fadeInfos_[kFadePlaceMapShow], GameFadeEffect::kStateFadeIn);
			state_ = kStateField;
		}
		break;
	case kStateSystemMenuStart:
		if (fadeEffect_->getState() == GameFadeEffect::kStateFadeOutEnd) {
			state_ = kStateSystemMenu;
			systemMenu_->start();
			fadeEffect_->start(GameFadeEffect::kTypeFade, GameFadeEffect::kStateFadeIn);
			gameMap_->freeze(true);
			gameCollision_->freeze(true);
			gameEventManager_->freeze(true);
			if (!gamePlayers_.empty())
				gamePlayers_[0]->freeze(true);
		}
		break;
	case kStateSystemMenu:
		if (systemMenu_->isEnd()) {
			state_ = kStateSystemMenuEnd;
			systemMenu_->pauseUpdate(true);
			fadeEffect_->start(GameFadeEffect::kTypeFade, GameFadeEffect::kStateFadeOut);
		}
		break;
	case kStateSystemMenuEnd:
		if (fadeEffect_->getState() == GameFadeEffect::kStateFadeOutEnd) {
			endSystemMenu();
		}
		break;
	}
}

void GameField::draw()
{
}

GamePlayer* GameField::getPlayerFromId(int playerId)
{
	for (u32 i = 0; i < gamePlayers_.size(); i++) {
		if (gamePlayers_[i]->getPlayerId() == playerId)
			return gamePlayers_[i];
	}
	return NULL;
}

void GameField::addPlayer(int playerId)
{
	if (!getPlayerFromId(playerId)) {
		GamePlayer* player = GamePlayer::createTask(this, playerId, gameSystem_.getPlayerStatus(playerId));
		gamePlayers_.push_back(player);
		if (gamePlayers_.size() == 1) {
			gameCollision_->addChara(gamePlayers_[0]);
			gamePlayers_[0]->setPosition(dummyLeader_->getPosition());
			gamePlayers_[0]->setDirection(dummyLeader_->getDirection());
		} else {
			player->pauseUpdate(true);
		}
	}
}

void GameField::removePlayer(int playerId)
{
	for (u32 i = 0; i < gamePlayers_.size(); i++) {
		if (gamePlayers_[i]->getPlayerId() == playerId) {
			if (i == 0)
				gameCollision_->removeChara(gamePlayers_[i]);
			gamePlayers_.erase(gamePlayers_.begin() + i);
			if (i == 0 && !gamePlayers_.empty()) {
				gameCollision_->addChara(gamePlayers_[0]);
				gamePlayers_[0]->pauseUpdate(false);
			}
			break;
		}
	}
}

void GameField::startBattle(const std::string& terrain, int enemyGroupId, bool firstAttack, bool enableEscape, bool loseGameOver)
{
	state_ = kStateBattleStart;
	battleLoseGameOver_ = loseGameOver;
	gameBattle_ = GameBattle::createTask(this, gameSystem_, terrain, enemyGroupId);
	gameBattle_->setFirstAttack(firstAttack);
	gameBattle_->setEnableEscape(enableEscape);
	for (u32 i = 0; i < gamePlayers_.size(); i++) {
		gameBattle_->addPlayer(gamePlayers_[i]->getPlayerId(), gamePlayers_[i]->getStatus());
	}
	gameBattle_->freeze(true);
	fadeEffect_->start((GameFadeEffect::FadeType)fadeInfos_[kFadePlaceBattleStartHide], GameFadeEffect::kStateEncountFlash);
	gameMap_->pauseUpdate(true);
	gameCollision_->pauseUpdate(true);
	gameEventManager_->pauseUpdate(true);
	if (!gamePlayers_.empty())
		gamePlayers_[0]->pauseUpdate(true);
}

void GameField::endBattle()
{
	state_ = kStateField;
	battleResult_ = gameBattle_->getResult();
	gameBattle_->release();
	gameBattle_ = NULL;
	fadeEffect_->start((GameFadeEffect::FadeType)fadeInfos_[kFadePlaceBattleEndShow], GameFadeEffect::kStateFadeIn);
	if (battleLoseGameOver_ && battleResult_ == GameBattle::kResultLose) {
		gameOver();
	} else {
		gameMap_->pauseUpdate(false);
		gameMap_->freeze(false);
		gameCollision_->pauseUpdate(false);
		gameCollision_->freeze(false);
		gameEventManager_->pauseUpdate(false);
		gameEventManager_->freeze(false);
		if (!gamePlayers_.empty()) {
			gamePlayers_[0]->pauseUpdate(false);
			gamePlayers_[0]->freeze(false);
		}
	}
}

void GameField::gameOver()
{
	game_->gameOver();
}

void GameField::returnTitle()
{
	game_->returnTitle();
}

void GameField::changeMap(int mapId, int x, int y, int dir)
{
	mapChangeInfo_.mapId = mapId;
	mapChangeInfo_.x = x;
	mapChangeInfo_.y = y;
	mapChangeInfo_.dir = dir;
	
	state_ = kStateChangeMap;
	fadeEffect_->start((GameFadeEffect::FadeType)fadeInfos_[kFadePlaceMapHide], GameFadeEffect::kStateFadeOut);
	gameMap_->pauseUpdate(true);
	gameCollision_->pauseUpdate(true);
	gameEventManager_->pauseUpdate(true);
	if (!gamePlayers_.empty())
		gamePlayers_[0]->pauseUpdate(true);
}

void GameField::fadeOut(int type)
{
	fadeEffectScreen_->start((GameFadeEffect::FadeType)(type == -1? fadeInfos_[kFadePlaceMapHide] : type), GameFadeEffect::kStateFadeOut);
}

void GameField::fadeIn(int type)
{
	fadeEffectScreen_->start((GameFadeEffect::FadeType)(type == -1? fadeInfos_[kFadePlaceMapShow] : type), GameFadeEffect::kStateFadeIn);
}

void GameField::startSystemMenu()
{
	state_ = kStateSystemMenuStart;
	fadeEffect_->start(GameFadeEffect::kTypeFade, GameFadeEffect::kStateFadeOut);
	gameMap_->pauseUpdate(true);
	gameCollision_->pauseUpdate(true);
	gameEventManager_->pauseUpdate(true);
	if (!gamePlayers_.empty())
		gamePlayers_[0]->pauseUpdate(true);
}

void GameField::endSystemMenu()
{
	state_ = kStateField;
	systemMenu_->freeze(true);
	systemMenu_->pauseUpdate(false);
	fadeEffect_->start(GameFadeEffect::kTypeFade, GameFadeEffect::kStateFadeIn);
	gameMap_->pauseUpdate(false);
	gameMap_->freeze(false);
	gameCollision_->pauseUpdate(false);
	gameCollision_->freeze(false);
	gameEventManager_->pauseUpdate(false);
	gameEventManager_->freeze(false);
	if (!gamePlayers_.empty()) {
		gamePlayers_[0]->pauseUpdate(false);
		gamePlayers_[0]->freeze(false);
	}
}




