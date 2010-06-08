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
	resetPlayerInfoList();
}

void GameSystem::resetPlayerInfoList()
{
	playerInfoList_.clear();
	playerInfoList_.resize(rpgLdb_.saPlayer.GetSize());
	CRpgLdb::Status itemUp = {0, 0, 0, 0, 0, 0};
	for (uint playerId = 1; playerId < rpgLdb_.saPlayer.GetSize(); playerId++) {
		const CRpgLdb::Player& player = rpgLdb_.saPlayer[playerId];
		playerInfoList_[playerId].baseInfo = &player;
		playerInfoList_[playerId].status.setPlayerStatus(rpgLdb_, playerId, player.startLevel, itemUp, player.initEquip);
		playerInfoList_[playerId].name = player.name;
		playerInfoList_[playerId].title = player.title;
		playerInfoList_[playerId].walkGraphicName = player.walkGraphicName;
		playerInfoList_[playerId].walkGraphicPos = player.walkGraphicPos;
		playerInfoList_[playerId].walkGraphicSemi = player.walkGraphicSemi;		
		playerInfoList_[playerId].faceGraphicName = player.faceGraphicName;
		playerInfoList_[playerId].faceGraphicPos = player.faceGraphicPos;
	}
}



