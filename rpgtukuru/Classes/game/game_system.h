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
#include "CRpgLdb.h"
#include "CRpgLmt.h"
#include "game_chara_status.h"
#include "game_config.h"

class GameInventory;
class GameCharaStatus;


class GameSystem
{
public:
	typedef std::vector<GameCharaStatus> PlayerStatusList;
	
public:
	GameSystem(const char* folder);
	~GameSystem();

public:
	void reset();
	
	const std::string& getRootFolder() const { return rpgLdb_.getRootFolder(); }
	const CRpgLdb& getRpgLdb() const { return rpgLdb_; }
	CRpgLdb& getRpgLdb() { return rpgLdb_; }
	const CRpgLmt& getRpgLmt() const { return rpgLmt_; }
	CRpgLmt& getRpgLmt() { return rpgLmt_; }
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
	PlayerStatusList& getPlayerStatusList() { return playerStatusList_; }
	GameCharaStatus& getPlayerStatus(int playerId) { return playerStatusList_[playerId]; }
	const GameCharaStatus& getPlayerStatus(int playerId) const { return playerStatusList_[playerId]; }
	GameConfig& getConfig() { return config_; }
	const GameConfig& getConfig() const { return config_; }	

private:
	void resetPlayerStatusList();

private:
	CRpgLdb							rpgLdb_;
	CRpgLmt							rpgLmt_;
	kuto::Texture					systemTexture_;
	kuto::ScopedPtr<GameInventory>	inventory_;
	kuto::Array<bool, 5000>			switchArray_;
	kuto::Array<int, 5000>			varArray_;
	PlayerStatusList				playerStatusList_;
	int								saveCount_;
	int								battleCount_;
	int								winCount_;
	int								loseCount_;
	int								escapeCount_;
	GameConfig						config_;
};	// class GameSystem
