/**
 * @file
 * @brief System
 * @author project.kuto
 */

#include "game_system.h"
#include "game_inventory.h"


GameSystem::GameSystem(const char* folder)
: saveCount_(0), battleCount_(0), winCount_(0), loseCount_(0), escapeCount_(0)
{
	if (!rpgLdb_.Init(folder)) {
		kuto_printf("error: cannot open RPG_RT.ldb¥n");
		return;
	}
	
	if (!rpgLmt_.Init(folder)) {
		kuto_printf("error: cannot open RPG_RT.lmt¥n");
		return;
	}
	
	std::string systemName = rpgLdb_.getRootFolder();
	systemName += "/System/" + rpgLdb_.system.system;
	CRpgUtil::LoadImage(systemTexture_, systemName, true);
	
	reset();
}

GameSystem::~GameSystem()
{
}

void GameSystem::reset()
{
	switchArray_.zeromemory();
	varArray_.zeromemory();
	inventory_.reset(new GameInventory(rpgLdb_));
	saveCount_ = 0;
	battleCount_ = 0;
	winCount_ = 0;
	loseCount_ = 0;
	escapeCount_ = 0;
	resetPlayerStatusList();
}

void GameSystem::resetPlayerStatusList()
{
	playerStatusList_.clear();
	playerStatusList_.resize(rpgLdb_.saPlayer.GetSize());
	CRpgLdb::Status itemUp = {0, 0, 0, 0, 0, 0};
	for (u32 playerId = 1; playerId < rpgLdb_.saPlayer.GetSize(); playerId++) {
		const CRpgLdb::Player& player = rpgLdb_.saPlayer[playerId];
		playerStatusList_[playerId].setPlayerStatus(rpgLdb_, playerId, player.startLevel, itemUp, player.initEquip);
	}
}



