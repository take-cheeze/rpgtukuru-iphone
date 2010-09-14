/**
 * @file
 * @brief Game System Menu
 * @author project.kuto
 */

#include <kuto/kuto_render_manager.h>
#include <kuto/kuto_graphics2d.h>

#include "game.h"

#include "game_system_menu.h"
#include "game_system.h"
#include "game_field.h"
#include "game_select_window.h"
#include "game_message_window.h"
#include "game_inventory.h"
#include "game_player.h"
#include "game_save_menu.h"
#include "game_equip_menu.h"
#include "game_item_menu.h"
#include "game_skill_menu.h"
#include "game_chara_select_menu.h"


GameSystemMenu::GameSystemMenu(GameField* gameField)
: kuto::IRender2D(kuto::Layer::OBJECT_2D, 20.f)
, gameField_(gameField), state_(kStateNone), childMenu_(NULL)
{
	const rpg2k::model::DataBase& ldb = gameField_->getGameSystem().getLDB();
	topMenu_ = addChild(GameSelectWindow::createTask(gameField_->getGameSystem()));
	topMenu_->pauseUpdate(true);
	topMenu_->addLine(ldb.vocabulary(106).toSystem());
	topMenu_->addLine(ldb.vocabulary(107).toSystem());
	topMenu_->addLine(ldb.vocabulary(108).toSystem());
	topMenu_->addLine(ldb.vocabulary(110).toSystem());
	topMenu_->addLine(ldb.vocabulary(112).toSystem());
	topMenu_->setPosition(kuto::Vector2(0.f, 0.f));
	topMenu_->setSize(kuto::Vector2(87.f, 96.f));
	topMenu_->setAutoClose(false);

	charaMenu_ = addChild(kuto::TaskCreatorParam1<GameCharaSelectMenu, GameField*>::createTask(gameField_));
	charaMenu_->pauseUpdate(true);
	charaMenu_->setPosition(kuto::Vector2(87.f, 0.f));
	charaMenu_->setSize(kuto::Vector2(233.f, 240.f));
	charaMenu_->setAutoClose(false);
	charaMenu_->setPauseUpdateCursor(true);
	charaMenu_->setShowCursor(false);

	moneyWindow_ = addChild(GameMessageWindow::createTask(gameField_->getGameSystem()));
	moneyWindow_->pauseUpdate(true);
	moneyWindow_->setPosition(kuto::Vector2(0.f, 208.f));
	moneyWindow_->setSize(kuto::Vector2(87.f, 32.f));
	moneyWindow_->setMessageAlign(GameWindow::kAlignRight);
	moneyWindow_->setEnableClick(false);
	updateMoneyWindow();
}

bool GameSystemMenu::initialize()
{
	if (isInitializedChildren()) {
		topMenu_->pauseUpdate(false);
		charaMenu_->pauseUpdate(false);
		moneyWindow_->pauseUpdate(false);
		freeze(true);
		return true;
	}
	return false;
}

void GameSystemMenu::update()
{
	switch (state_) {
	case kStateTop:
		if (topMenu_->selected()) {
			switch (topMenu_->cursor()) {
			case kTopMenuItem:
				state_ = kStateChild;
				childMenu_ = GameItemMenu::createTask(gameField_);
				topMenu_->freeze(true);
				moneyWindow_->freeze(true);
				charaMenu_->freeze(true);
				break;
			case kTopMenuSkill:
				state_ = kStateChara;
				topMenu_->setPauseUpdateCursor(true);
				charaMenu_->setPauseUpdateCursor(false);
				charaMenu_->setShowCursor(true);
				charaMenu_->reset();
				break;
			case kTopMenuEquip:
				state_ = kStateChara;
				topMenu_->setPauseUpdateCursor(true);
				charaMenu_->setPauseUpdateCursor(false);
				charaMenu_->setShowCursor(true);
				charaMenu_->reset();
				break;
			case kTopMenuSave:
				state_ = kStateChild;
				childMenu_ = addChild(GameSaveMenu::createTask(gameField_));
				topMenu_->freeze(true);
				moneyWindow_->freeze(true);
				charaMenu_->freeze(true);
				break;
			case kTopMenuEndGame:
				gameField_->getGame()->returnTitle();
				break;
			}
		} else if (topMenu_->canceled()) {
			state_ = kStateNone;
		}
		break;
	case kStateChara:
		if (charaMenu_->selected()) {
			GameCharaStatus& status = gameField_->getPlayers()[charaMenu_->cursor()]->getStatus();
			switch (topMenu_->cursor()) {
			case kTopMenuSkill:
				state_ = kStateChild;
				childMenu_ = GameSkillMenu::createTask(gameField_, &status);
				topMenu_->freeze(true);
				moneyWindow_->freeze(true);
				charaMenu_->freeze(true);
				break;
			case kTopMenuEquip:
				state_ = kStateChild;
				childMenu_ = GameEquipMenu::createTask(gameField_, &status);
				topMenu_->freeze(true);
				moneyWindow_->freeze(true);
				charaMenu_->freeze(true);
				break;
			}
		} else if (charaMenu_->canceled()) {
			state_ = kStateTop;
			topMenu_->setPauseUpdateCursor(false);
			charaMenu_->setPauseUpdateCursor(true);
			charaMenu_->setShowCursor(false);
			topMenu_->reset();
		}
		break;
	case kStateChild:
		if (childMenu_->isInitialized() && childMenu_->isEnd()) {
			state_ = childMenu_->isSystemMenuEnd()? kStateNone : kStateTop;
			childMenu_->release();
			childMenu_ = NULL;
			topMenu_->freeze(false);
			moneyWindow_->freeze(false);
			charaMenu_->freeze(false);
			topMenu_->setPauseUpdateCursor(false);
			charaMenu_->setPauseUpdateCursor(true);
			charaMenu_->setShowCursor(false);
			topMenu_->reset();
		}
		break;
	default: break;
	}
}

void GameSystemMenu::updateMoneyWindow()
{
	const rpg2k::model::DataBase& ldb = gameField_->getGameSystem().getLDB();
	moneyWindow_->clearMessages();
	char temp[256];
	sprintf(temp, "%d%s", gameField_->getGameSystem().getLSD().getMoney(), ldb.vocabulary(15).toSystem().c_str());
	moneyWindow_->addLine(temp);
}

void GameSystemMenu::start()
{
	freeze(false);
	state_ = kStateTop;
	topMenu_->reset();
	topMenu_->resetCursor();
	updateMoneyWindow();
}

void GameSystemMenu::render(kuto::Graphics2D* g) const
{
}
