/**
 * @file
 * @brief Game Title
 * @author project.kuto
 */

#include "game_title.h"
#include <kuto/kuto_render_manager.h>
#include <kuto/kuto_graphics2d.h>
#include <kuto/kuto_error.h>
#include <kuto/kuto_utility.h>
#include <kuto/kuto_file.h>
#include <kuto/kuto_virtual_pad.h>
#include "game_select_window.h"
#include "game_load_menu.h"


GameTitle::GameTitle(rpg2k::model::Project& gameSystem)
: kuto::IRender2D(kuto::Layer::OBJECT_2D, 20.f)
, gameSystem_(gameSystem)
, screenOffset_(0.f, 0.f), screenScale_(1.f, 1.f), selectMenu_(kSelectNone)
{
	bool res = RPG2kUtil::LoadImage(titleTex_, std::string( gameSystem_.gameDir() ).append("/Title/").append(gameSystem_.getLDB().system()[17].get_string().toSystem()), false); kuto_assert(res);

	selectWindow_ = addChild(GameSelectWindow::createTask(gameSystem_));
	selectWindow_->setPosition(kuto::Vector2(110.f, 150.f));
	selectWindow_->setSize(kuto::Vector2(100.f, 64.f));
	selectWindow_->addLine(gameSystem_.getLDB().vocabulary(114).toSystem());
	selectWindow_->addLine(gameSystem_.getLDB().vocabulary(115).toSystem());
	selectWindow_->addLine(gameSystem_.getLDB().vocabulary(117).toSystem());
	selectWindow_->setEnableCancel(false);
	selectWindow_->setAutoClose(false);

	loadMenu_ = addChild(kuto::TaskCreatorParam1<GameLoadMenu, rpg2k::model::Project&>::createTask(gameSystem_));
}

bool GameTitle::initialize()
{
	return isInitializedChildren();
}

void GameTitle::update()
{
	if (loadMenu_->isFreeze()) {
		if (selectWindow_->selected()) {
			switch (selectWindow_->cursor()) {
			case kSelectContinue:
				loadMenu_->start();
				selectWindow_->freeze(true);
				break;
			default:
				selectMenu_ = (SelectMenuType)selectWindow_->cursor();
				break;
			}
		}
	} else {
		if (loadMenu_->selected()) {
			selectMenu_ = kSelectContinue;
		} else if (loadMenu_->canceled()) {
			loadMenu_->freeze(true);
			selectWindow_->freeze(false);
			selectWindow_->reset();
		}
	}
}

void GameTitle::render(kuto::Graphics2D* g) const
{
	if (!loadMenu_->isFreeze()) return;

	const kuto::Color color(1.f, 1.f, 1.f, 1.f);
	kuto::Vector2 pos(screenOffset_);
	kuto::Vector2 scale(titleTex_.getOrgWidth(), titleTex_.getOrgHeight());
	scale *= screenScale_;
	g->drawTexture(titleTex_, pos, scale, color, true);
}

int GameTitle::getSaveId() const
{
	return loadMenu_->selectSaveId();
}


