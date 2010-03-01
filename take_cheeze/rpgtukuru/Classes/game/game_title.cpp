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
#include "game_image.h"
#include "game_select_window.h"
#include "game_load_menu.h"

using namespace rpg2kLib::structure;


GameTitle::GameTitle(kuto::Task* parent, GameSystem& gameSystem)
: kuto::Task(parent)
, gameSystem_(gameSystem)
, screenOffset_(0.f, 0.f), screenScale_(1.f, 1.f), selectMenu_(kSelectNone)
{
	std::string titleName = gameSystem_.getRootFolder();
	titleName += "/Title/" + gameSystem_.getRpgLdb().getSystem()[17].get_string();
	GameImage::LoadImage(titleTex_, titleName, false);

	const Array1D& voc = gameSystem_.getRpgLdb().getVocabulary();

	selectWindow_ = GameSelectWindow::createTask(this, gameSystem_);
	selectWindow_->setPosition(kuto::Vector2(110.f, 150.f));
	selectWindow_->setSize(kuto::Vector2(100.f, 64.f));
	selectWindow_->addMessage(voc[0x72]);
	selectWindow_->addMessage(voc[0x73]);
	selectWindow_->addMessage(voc[0x75]);
	selectWindow_->setEnableCancel(false);
	selectWindow_->setAutoClose(false);
	
	loadMenu_ = GameLoadMenu::createTask(this, gameSystem_);
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

void GameTitle::draw()
{
	if (loadMenu_->isFreeze())
		kuto::RenderManager::instance()->addRender(this, kuto::LAYER_2D_OBJECT, 20.f);
}

void GameTitle::render(kuto::Graphics2D& g)
{
	const kuto::Color color(1.f, 1.f, 1.f, 1.f);
	kuto::Vector2 pos(screenOffset_);
	kuto::Vector2 scale(titleTex_.getOrgWidth(), titleTex_.getOrgHeight());
	scale *= screenScale_;
	g.drawTexture(titleTex_, pos, scale, color, true);
}

int GameTitle::getSaveId() const
{
	return loadMenu_->selectSaveId();
}


