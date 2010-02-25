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
#include "game_image.h"
#include "game_over.h"
#include "game.h"


GameOver::GameOver(Game* parent, GameSystem& gameSystem)
: kuto::Task(parent)
, game_(parent), gameSystem_(gameSystem)
{
	std::string texName = gameSystem_.getRootFolder();
	texName += "/GameOver/" + gameSystem_.getRpgLdb().getSystem()[18].get_string();
	GameImage::LoadImage(texture_, texName, false);
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

void GameOver::draw()
{
	kuto::RenderManager::instance()->addRender(this, kuto::LAYER_2D_OBJECT, 20.f);
}

void GameOver::render()
{
	kuto::Graphics2D* g = kuto::RenderManager::instance()->getGraphics2D();
	const kuto::Color color(1.f, 1.f, 1.f, 1.f);
	kuto::Vector2 pos(0.f, 0.f);
	kuto::Vector2 scale(texture_.getOrgWidth(), texture_.getOrgHeight());
	g->drawTexture(texture_, pos, scale, color, true);
}
