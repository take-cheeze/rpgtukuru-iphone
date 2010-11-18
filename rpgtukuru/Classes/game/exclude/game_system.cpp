/**
 * @file
 * @brief System
 * @author project.kuto
 */

#include "game_system.h"
#include "game_inventory.h"

#include <kuto/kuto_graphics2d.h>

#include <boost/smart_ptr.hpp>

#include <map>
#include <utility>


kuto::Texture& getSystemTexture(rpg2k::model::Project const& proj)
{
	static std::map< std::string, boost::shared_ptr< kuto::Texture > > cache_;

	rpg2k::RPG2kString const& key = proj.systemGraphic();
	if( cache_.find(key) == cache_.end() ) {
		kuto::Texture* newed = new kuto::Texture;
		if( !RPG2kUtil::LoadImage(*newed, std::string(proj.gameDir())
		.append("/System/").append(key.toSystem()), true) ) kuto_assert(false);
		cache_.insert( std::make_pair(key, newed) );
		return *newed;
	} else return *(cache_.find(key)->second);
}

/*
GameSystem::GameSystem(const char* folder)
: rpgLdb_(folder), rpgLmt_(folder)
, saveCount_(0), battleCount_(0), winCount_(0), loseCount_(0), escapeCount_(0)
{
	if (!rpgLdb_.Init(folder)) {
		kuto_printf("error: cannot open RPG_RT.ldb¥n");
		return;
	}

	if (!rpgLmt_.Init(folder)) {
		kuto_printf("error: cannot open RPG_RT.lmt¥n");
		return;
	}

	std::string systemName = rpgLdb_.gameDir();
	systemName += "/System/" + rpgLdb_.system.system;
	bool res = RPG2kUtil::LoadImage(systemTexture_, systemName, true); kuto_assert(res);

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
	playerInfoList_.resize(rpgLdb_.character().rend().first() + 1);
	std::vector< uint16_t > itemUp(5, 0);
	const rpg2k::structure::Array2D& charList = rpgLdb_.character();
	for (rpg2k::structure::Array2D::Iterator it = charList.begin(); it != charList.end(); ++it) {
		int playerId = it->first;
		playerInfoList_[playerId].baseInfo = &it->second;
		playerInfoList_[playerId].status.setPlayerStatus(rpgLdb_, playerId, (*it->second)[7].to<int>(), itemUp, (*it->second)[51].getBinary());
		playerInfoList_[playerId].name = (*it->second)[1].to_string().toSystem();
		playerInfoList_[playerId].title = (*it->second)[2].to_string().toSystem();
		playerInfoList_[playerId].walkGraphicName = (*it->second)[3].to_string().toSystem();
		playerInfoList_[playerId].walkGraphicPos = (*it->second)[4].to<int>();
		playerInfoList_[playerId].walkGraphicSemi = (*it->second)[5].to<int>();
		playerInfoList_[playerId].faceGraphicName = (*it->second)[15].to_string().toSystem();
		playerInfoList_[playerId].faceGraphicPos = (*it->second)[16].to<int>();
	}
}
 */
