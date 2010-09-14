/**
 * @file
 * @brief Save Data
 * @author project.kuto
 */

#include <cstring>
#include "game_save_data.h"
#include "game_field.h"
#include "game_map.h"
#include "game_player.h"
#include "game_npc.h"
#include "game_inventory.h"
#include "game_event_picture.h"
#include "game_system.h"


void GameSaveDataHeader::save(GameField* gameField)
{
/*
	const rpg2k::model::DataBase& ldb = gameField->getGameSystem().getLDB();
	time_ = (uint64_t)time(NULL);
	leaderName_ = gameField->getGameSystem().getPlayerInfo(gameField->getPlayerLeader()->getPlayerId()).name;
	leaderLevel_ = gameField->getPlayerLeader()->getStatus().getLevel();
	leaderHp_ = gameField->getPlayerLeader()->getStatus().getHp();
	partyNum_ = gameField->getPlayers().size();
	for (uint i = 0; i < 4; i++) {
		if (i < gameField->getPlayers().size()) {
			partyId_[i] = gameField->getPlayers()[i]->getPlayerId();
			partyImage_[i] = ldb.character()[gameField->getPlayers()[i]->getPlayerId()][15].get_string().toSystem();
			partyImageNo_[i] = ldb.character()[gameField->getPlayers()[i]->getPlayerId()][16].get<int>();
		} else {
			partyId_[i] = 0;
			partyImage_[i].clear();
			partyImageNo_[i] = 0;
		}
	}
 */
}


void GameSaveDataSystem::save(GameField* gameField)
{
/*
	const rpg2k::model::Project& gameSystem = gameField->getGameSystem();
	const rpg2k::model::DataBase& ldb = gameSystem.getLDB();
	systemTexture_ = ldb.system()[19].get_string().toSystem();
	for (int i = 0; i < 5000; i++) {
		switches_[i] = gameSystem.getSwitch(i);
		vars_[i] = gameSystem.getVar(i);
	}
	enableTeleport_ = true;
	enableEscape_ = true;
	enableSave_ = true;
	enableMenu_ = true;
	battleMap_.clear();
	saveCount_ = gameSystem.getSaveCount();
	battleCount_ = gameSystem.getBattleCount();
	winCount_ = gameSystem.getWinCount();
	loseCount_ = gameSystem.getLoseCount();
	escapeCount_ = gameSystem.getEscapeCount();
 */
}

void GameSaveDataSystem::load(GameField* gameField)
{
/*
	rpg2k::model::Project& gameSystem = gameField->getGameSystem();
	const rpg2k::model::DataBase& ldb = gameSystem.getLDB();
	systemTexture_ = ldb.system()[19].get_string().toSystem();
	for (int i = 0; i < 5000; i++) {
		gameSystem.setSwitch(i, switches_[i]);
		gameSystem.setVar(i, vars_[i]);
	}
	//enableTeleport_ = true;
	//enableEscape_ = true;
	//enableSave_ = true;
	//enableMenu_ = true;
	//std::strcpy(battleMap_, "");
	gameSystem.setSaveCount(saveCount_);
	gameSystem.setBattleCount(battleCount_);
	gameSystem.setWinCount(winCount_);
	gameSystem.setLoseCount(loseCount_);
	gameSystem.setEscapeCount(escapeCount_);
 */
}


void GameSaveDataLocation::save(GameField* gameField)
{
	mapId_ = gameField->getMap()->getMapId();
	x_ = gameField->getPlayerLeader()->getPosition().x;
	y_ = gameField->getPlayerLeader()->getPosition().y;
	dir_ = gameField->getPlayerLeader()->getDirection();
}


void GameSaveDataInventory::save(GameField* gameField)
{
/*
	const GameInventory* inventory = gameField->getGameSystem().getInventory();
	money_ = inventory->getMoney();
	std::memset(items_, 0, sizeof(items_));
	for (uint i = 0; i < inventory->getItemList().size(); i++) {
		items_[i] = (char)inventory->getItemList()[i];
	}
 */
}

void GameSaveDataInventory::load(GameField* gameField)
{
/*
	GameInventory* inventory = gameField->getGameSystem().getInventory();
	inventory->setMoney(money_);
	for (uint i = 0; i < inventory->getItemList().size(); i++) {
		inventory->setItemNum(i, items_[i]);
	}
 */
}


void GameSaveDataPlayers::save(GameField* gameField)
{
/*
	playerNum_ = gameField->getGameSystem().getPlayerInfoList().size();
	for (int i = 0; i < playerNum_; i++) {
		playerInfos_[i].status = gameField->getGameSystem().getPlayerInfoList()[i].status;
	}
 */
}

void GameSaveDataPlayers::load(GameField* gameField)
{
/*
	for (uint i = 1; i < gameField->getGameSystem().getPlayerInfoList().size(); i++) {
		static_cast<GameCharaStatusBase&>(gameField->getGameSystem().getPlayerInfoList()[i].status) = playerInfos_[i].status;
		gameField->getGameSystem().getPlayerInfoList()[i].status.calcStatus(false);
	}
 */
}


void GameSaveData::save(GameField* gameField)
{
	header_.save(gameField);
	system_.save(gameField);
	location_.save(gameField);
	inventory_.save(gameField);
	players_.save(gameField);
}

void GameSaveData::load(GameField* gameField)
{
	system_.load(gameField);
	inventory_.load(gameField);
	players_.load(gameField);
}
