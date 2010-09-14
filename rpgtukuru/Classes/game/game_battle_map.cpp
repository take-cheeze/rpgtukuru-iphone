/**
 * @file
 * @brief Game Battle Map
 * @author project.kuto
 */

#include <kuto/kuto_render_manager.h>
#include <kuto/kuto_graphics2d.h>
#include "game_battle_map.h"
#include "game_battle.h"


GameBattleMap::GameBattleMap(const rpg2k::model::Project& gameSystem, const std::string& terrain)
: kuto::IRender2D(kuto::Layer::OBJECT_2D, 10.f), gameSystem_(gameSystem), animationCounter_(0)
{
	bool res = RPG2kUtil::LoadImage(texture_, std::string( gameSystem_.gameDir() ).append("/Backdrop/").append(terrain), false); kuto_assert(res);
}

bool GameBattleMap::initialize()
{
	return isInitializedChildren();
}

void GameBattleMap::update()
{
	animationCounter_++;
}

void GameBattleMap::render(kuto::Graphics2D* g) const
{
	const kuto::Color color(1.f, 1.f, 1.f, 1.f);
	kuto::Vector2 pos(0.f, 0.f);
	pos += static_cast<GameBattle const*>(getParent())->getScreenOffset();
	kuto::Vector2 scale(texture_.getOrgWidth(), texture_.getOrgHeight());
	g->drawTexture(texture_, pos, scale, color, true);
}

