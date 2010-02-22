/**
 * @file
 * @brief System
 * @author project.kuto
 */

#include "game_image.h"
#include "game_inventory.h"
#include "game_system.h"

GameSystem::GameSystem(const char* folder)
: rpgLdb_(folder), rpgLmt_(folder)
, saveCount_(0), battleCount_(0), winCount_(0), loseCount_(0), escapeCount_(0)
{
	std::string systemName = rpgLdb_.getRootFolder();
	systemName += "/System/" + rpgLdb_.getSystem()[19].get_string();
	GameImage::LoadImage(systemTexture_, systemName, true);

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
	const Array2D& charaList = rpgLdb_.getCharacter();

	playerStatusList_.resize( charaList.end().first()+1 );
	Status itemUp = {0, 0, 0, 0, 0, 0};

	for (Array2D::Iterator it = charaList.begin(); it != charaList.end(); ++it) {
		const Array1D& player = it.second();
		vector< uint16_t > equipVec = player[51].getBinary();
		Equip e = { equipVec[0], equipVec[0], equipVec[0], equipVec[0], equipVec[0] };

		playerStatusList_[it.first()].setPlayerStatus(rpgLdb_, it.first(), player[7], itemUp, e);
	}
}



