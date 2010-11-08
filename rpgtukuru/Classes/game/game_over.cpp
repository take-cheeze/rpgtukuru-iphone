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


GameOver::GameOver(Game& g)
: kuto::IRender2D(kuto::Layer::OBJECT_2D, 20.f)
, game_(g), project_( g.project() )
{
	bool res = RPG2kUtil::LoadImage(texture_, std::string(project_.gameDir()).append("/GameOver/").append( project_.getLDB().system()[18].to_string().toSystem() ), false); kuto_assert(res);
}

bool GameOver::initialize()
{
	return isInitializedChildren();
}

void GameOver::update()
{
	kuto::VirtualPad& virtualPad = kuto::VirtualPad::instance();
	if (virtualPad.repeat(kuto::VirtualPad::KEY_A)) {
		game_.returnTitle();
	}
}

void GameOver::render(kuto::Graphics2D& g) const
{
	const kuto::Color color(1.f, 1.f, 1.f, 1.f);
	kuto::Vector2 pos(0.f, 0.f);
	kuto::Vector2 scale(texture_.orgWidth(), texture_.orgHeight());
	g.drawTexture(texture_, pos, scale, color, true);
}
