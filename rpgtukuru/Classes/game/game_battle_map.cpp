/**
 * @file
 * @brief Game Battle Map
 * @author project.kuto
 */

#include <kuto/kuto_render_manager.h>
#include <kuto/kuto_graphics2d.h>
#include "game_battle_map.h"
#include "game_battle.h"
#include "CRpgUtil.h"


GameBattleMap::GameBattleMap(kuto::Task* parent, const GameSystem& gameSystem, const std::string& terrain)
: kuto::Task(parent), gameSystem_(gameSystem), animationCounter_(0)
{
	std::string background = gameSystem_.getRootFolder();
	background += "/Backdrop/" + terrain;
	CRpgUtil::LoadImage(texture_, background, false);
}

bool GameBattleMap::initialize()
{
	return isInitializedChildren();
}

void GameBattleMap::update()
{
	animationCounter_++;
}

void GameBattleMap::draw()
{
	kuto::RenderManager::instance()->addRender(this, kuto::LAYER_2D_OBJECT, 10.f);
}

void GameBattleMap::render()
{
	kuto::Graphics2D* g = kuto::RenderManager::instance()->getGraphics2D();
	const kuto::Color color(1.f, 1.f, 1.f, 1.f);
	kuto::Vector2 pos(0.f, 0.f);
	pos += static_cast<GameBattle*>(getParent())->getScreenOffset();
	kuto::Vector2 scale(texture_.getOrgWidth(), texture_.getOrgHeight());
	g->drawTexture(texture_, pos, scale, color, true);
}

