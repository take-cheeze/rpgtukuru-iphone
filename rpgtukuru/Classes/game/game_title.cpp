/**
 * @file
 * @brief Game Title
 * @author project.kuto
 */

#include <kuto/kuto_error.h>
#include <kuto/kuto_file.h>
#include <kuto/kuto_graphics2d.h>
#include <kuto/kuto_render_manager.h>
#include <kuto/kuto_utility.h>
#include <kuto/kuto_virtual_pad.h>

#include "game.h"
#include "game_load_menu.h"
#include "game_select_window.h"
#include "game_title.h"

#include <rpg2k/Project.hpp>


GameTitle::GameTitle(Game& g)
: kuto::IRender2D(kuto::Layer::OBJECT_2D, 20.f)
, game_(g), project_( g.project() )
, screenOffset_(0.f, 0.f), screenScale_(1.f, 1.f)
, selectWindow_( *addChild(GameSelectWindow::createTask(game_)) )
, selectMenu_(kSelectNone)
{
	bool res = RPG2kUtil::LoadImage(titleTex_, std::string( project_.gameDir() ).append("/Title/").append(project_.getLDB().system()[17].to_string().toSystem()), false); kuto_assert(res);

	selectWindow_.setPosition(kuto::Vector2(110.f, 150.f));
	selectWindow_.setSize(kuto::Vector2(100.f, 64.f));
	selectWindow_.addLine(project_.getLDB().vocabulary(114).toSystem());
	selectWindow_.addLine(project_.getLDB().vocabulary(115).toSystem());
	selectWindow_.addLine(project_.getLDB().vocabulary(117).toSystem());
	selectWindow_.enableCancel(false);
	selectWindow_.setAutoClose(false);

	loadMenu_ = addChild(GameLoadMenu::createTask(g));
}

bool GameTitle::initialize()
{
	return isInitializedChildren();
}

void GameTitle::update()
{
	if (loadMenu_->isFreeze()) {
		if (selectWindow_.selected()) {
			switch (selectWindow_.cursor()) {
			case kSelectContinue:
				loadMenu_->start();
				selectWindow_.freeze();
				break;
			default:
				selectMenu_ = (SelectMenuType)selectWindow_.cursor();
				break;
			}
		}
	} else {
		if (loadMenu_->selected()) {
			selectMenu_ = kSelectContinue;
		} else if (loadMenu_->canceled()) {
			loadMenu_->freeze();
			selectWindow_.freeze(false);
			selectWindow_.reset();
		}
	}
}

void GameTitle::render(kuto::Graphics2D& g) const
{
	if (!loadMenu_->isFreeze()) return;

	const kuto::Color color(1.f, 1.f, 1.f, 1.f);
	kuto::Vector2 pos(screenOffset_);
	kuto::Vector2 scale(titleTex_.orgWidth(), titleTex_.orgHeight());
	scale *= screenScale_;
	g.drawTexture(titleTex_, pos, scale, color, true);
}

int GameTitle::saveID() const
{
	return loadMenu_->selectSaveId();
}


