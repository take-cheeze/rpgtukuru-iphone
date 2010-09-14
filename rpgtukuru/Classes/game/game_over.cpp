/**
 * @file
 * @brief Game Over
 * @author project.kuto
 */

#include <kuto/kuto_render_manager.h>
#include <kuto/kuto_graphics2d.h>
#include <kuto/kuto_error.h>
#include <kuto/kuto_utility.h>
#include <kuto/kuto_file.h>
#include <kuto/kuto_virtual_pad.h>

#include "game_over.h"
#include "game.h"


GameOver::GameOver(Game* parent, rpg2k::model::Project& gameSystem)
: kuto::IRender2D(kuto::Layer::OBJECT_2D, 20.f)
, game_(parent), gameSystem_(gameSystem)
{
	bool res = RPG2kUtil::LoadImage(texture_, std::string(gameSystem_.gameDir()).append("/GameOver/").append( gameSystem_.getLDB().system()[18].get_string().toSystem() ), false); kuto_assert(res);
}

bool GameOver::initialize()
{
	return isInitializedChildren();
}

void GameOver::update()
{
	kuto::VirtualPad* virtualPad = kuto::VirtualPad::instance();
	if (virtualPad->repeat(kuto::VirtualPad::KEY_A)) {
		game_->returnTitle();
	}
}

void GameOver::render(kuto::Graphics2D* g) const
{
	const kuto::Color color(1.f, 1.f, 1.f, 1.f);
	kuto::Vector2 pos(0.f, 0.f);
	kuto::Vector2 scale(texture_.getOrgWidth(), texture_.getOrgHeight());
	g->drawTexture(texture_, pos, scale, color, true);
}


