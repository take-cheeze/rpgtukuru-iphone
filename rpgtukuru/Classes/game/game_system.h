/**
 * @file
 * @brief System
 * @author project.kuto
 */
#pragma once

#include <vector>
#include <kuto/kuto_texture.h>
#include <kuto/kuto_math.h>
#include <kuto/kuto_scoped_ptr.h>
#include <kuto/kuto_array.h>
// #include <rpg2k/DataBase.hpp>
// #include <rpg2k/MapTree.hpp>
#include <rpg2k/Project.hpp>
#include "game_player_info.h"
#include "game_config.h"

kuto::Texture& getSystemTexture(rpg2k::model::Project const&);

/*
class GameInventory;


class GameSystem
{
public:
	typedef std::vector<GamePlayerInfo> PlayerInfoList;

public:
	GameSystem(const char* folder);
	~GameSystem();

public:
	void reset();

	const std::string& getGameDir() const { return rpgLdb_.directory(); }
	const rpg2k::model::DataBase& getLDB() const { return rpgLdb_; }
	rpg2k::model::DataBase& getLDB() { return rpgLdb_; }
	const rpg2k::model::MapTree& getRpgLmt() const { return rpgLmt_; }
	rpg2k::model::MapTree& getRpgLmt() { return rpgLmt_; }
	const kuto::Texture& getSystemTexture() const { return systemTexture_; }
	kuto::Texture& getSystemTexture() { return systemTexture_; }
	const GameInventory* getInventory() const { return inventory_.get(); }
	GameInventory* getInventory() { return inventory_.get(); }
	bool getSwitch(int index) const { return switchArray_[index]; }
	void setSwitch(int index, bool value) { switchArray_[index] = value; }
	int getVar(int index) const { return varArray_[index]; }
	void setVar(int index, int value) { varArray_[index] = value; }
	int getSaveCount() const { return saveCount_; }
	void setSaveCount(int value) { saveCount_ = value; }
	int getBattleCount() const { return battleCount_; }
	void setBattleCount(int value) { battleCount_ = value; }
	int getWinCount() const { return winCount_; }
	void setWinCount(int value) { winCount_ = value; }
	int getLoseCount() const { return loseCount_; }
	void setLoseCount(int value) { loseCount_ = value; }
	int getEscapeCount() const { return escapeCount_; }
	void setEscapeCount(int value) { escapeCount_ = value; }
	PlayerInfoList& getPlayerInfoList() { return playerInfoList_; }
	GamePlayerInfo& getPlayerInfo(int playerId) { return playerInfoList_[playerId]; }
	const GamePlayerInfo& getPlayerInfo(int playerId) const { return playerInfoList_[playerId]; }
	GameCharaStatus& getPlayerStatus(int playerId) { return playerInfoList_[playerId].status; }
	const GameCharaStatus& getPlayerStatus(int playerId) const { return playerInfoList_[playerId].status; }
	GameConfig& getConfig() { return config_; }
	const GameConfig& getConfig() const { return config_; }

private:
	void resetPlayerInfoList();

private:
	rpg2k::model::DataBase							rpgLdb_;
	rpg2k::model::MapTree							rpgLmt_;
	kuto::Texture					systemTexture_;
	kuto::ScopedPtr<GameInventory>	inventory_;
	kuto::Array<bool, 5000>			switchArray_;
	kuto::Array<int, 5000>			varArray_;
	PlayerInfoList					playerInfoList_;
	int								saveCount_;
	int								battleCount_;
	int								winCount_;
	int								loseCount_;
	int								escapeCount_;
	GameConfig						config_;
};	// class GameSystem
 */
