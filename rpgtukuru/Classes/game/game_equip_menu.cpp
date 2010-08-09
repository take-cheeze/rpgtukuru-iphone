/**
 * @file
 * @brief Save Menu
 * @author project.kuto
 */

#include <kuto/kuto_render_manager.h>
#include <kuto/kuto_graphics2d.h>
#include <kuto/kuto_file.h>
#include "game_equip_menu.h"
#include "game_system.h"
#include "game_field.h"
#include "game_select_window.h"
#include "game_message_window.h"
#include "game_chara_status.h"
#include "game_inventory.h"


GameEquipMenu::GameEquipMenu(GameField* gameField, GameCharaStatus* charaStatus)
: GameSystemMenuBase(gameField)
, charaStatus_(charaStatus)
{
	// const rpg2k::model::DataBase& ldb = gameField_->getGameSystem().getLDB();
	equipMenu_ = GameSelectWindow::createTask(this, gameField_->getGameSystem());
	equipMenu_->pauseUpdate(true);
	equipMenu_->setPosition(kuto::Vector2(124.f, 32.f));
	equipMenu_->setSize(kuto::Vector2(196.f, 96.f));
	equipMenu_->setAutoClose(false);

	itemMenu_ = GameSelectWindow::createTask(this, gameField_->getGameSystem());
	itemMenu_->pauseUpdate(true);
	itemMenu_->setPosition(kuto::Vector2(0.f, 128.f));
	itemMenu_->setSize(kuto::Vector2(320.f, 112.f));
	itemMenu_->setAutoClose(false);
	itemMenu_->setColumnSize(2);
	itemMenu_->setPauseUpdateCursor(true);
	itemMenu_->setShowCursor(false);

	statusWindow_ = GameMessageWindow::createTask(this, gameField_->getGameSystem());
	statusWindow_->pauseUpdate(true);
	statusWindow_->setPosition(kuto::Vector2(0.f, 32.f));
	statusWindow_->setSize(kuto::Vector2(124.f, 96.f));
	statusWindow_->setEnableClick(false);
	statusWindow_->setUseAnimation(false);
	statusWindow_->addLine(gameField_->getGameSystem().name(charaStatus_->getCharaId()));

	descriptionWindow_ = GameMessageWindow::createTask(this, gameField_->getGameSystem());
	descriptionWindow_->pauseUpdate(true);
	descriptionWindow_->setPosition(kuto::Vector2(0.f, 0.f));
	descriptionWindow_->setSize(kuto::Vector2(320.f, 32.f));
	descriptionWindow_->setEnableClick(false);
	descriptionWindow_->setUseAnimation(false);
	setDiscriptionMessage();
}

GameEquipMenu::~GameEquipMenu()
{
}

bool GameEquipMenu::initialize()
{
	if (isInitializedChildren()) {
		equipMenu_->pauseUpdate(false);
		itemMenu_->pauseUpdate(false);
		descriptionWindow_->pauseUpdate(false);
		statusWindow_->pauseUpdate(false);
		start();
		return true;
	}
	return false;
}

void GameEquipMenu::update()
{
	switch (state_) {
	case kStateEquip:
		setDiscriptionMessage();
		updateItemWindow();
		if (equipMenu_->selected()) {
			setState(kStateItem);
		} else if (equipMenu_->canceled()) {
			setState(kStateNone);
		}
		break;
	case kStateItem:
		setDiscriptionMessage();
		if (itemMenu_->selected()) {
			std::vector< uint16_t > equip = charaStatus_->getEquip();
			int oldId = equip[equipMenu_->cursor()];
			equip[equipMenu_->cursor()] = itemMenu_->cursor();
			charaStatus_->setEquip(equip);
			if (oldId > 0)
				gameField_->getGameSystem().getLSD().addItemNum(oldId, 1);
			if (itemList_[itemMenu_->cursor()] > 0)
				gameField_->getGameSystem().getLSD().addItemNum(itemList_[itemMenu_->cursor()], -1);
			setState(kStateEquip);
		} else if (itemMenu_->canceled()) {
			setState(kStateEquip);
		}
		break;
	}
}

void GameEquipMenu::setState(int newState)
{
	state_ = newState;
	const rpg2k::model::DataBase& ldb = gameField_->getGameSystem().getLDB();
	switch (state_) {
	case kStateEquip:
		{
			itemMenu_->setPauseUpdateCursor(true);
			itemMenu_->setShowCursor(false);
			equipMenu_->clearMessages();
			const std::vector< uint16_t >& equip = charaStatus_->getEquip();
			const rpg2k::structure::Array1D& player = ldb.character()[charaStatus_->getCharaId()];
			for(int i = rpg2k::Equip::BEGIN; i < rpg2k::Equip::END; i++) {
				if( (i == rpg2k::Equip::SHIELD) && player[21].get<bool>() ) equipMenu_->addLine(
					ldb.vocabulary(136) +
					(equip[i] ? ( " " + ldb.item()[ equip[i] ][1].get_string() ) : std::string())
				);
				else equipMenu_->addLine(
					ldb.vocabulary(136 + i) +
					(equip[i] ? ( " " + ldb.item()[ equip[i] ][1].get_string() ) : std::string())
				);
			}
			equipMenu_->setPauseUpdateCursor(false);
			equipMenu_->reset();
		}
		break;
	case kStateItem:
		itemMenu_->setPauseUpdateCursor(false);
		equipMenu_->setPauseUpdateCursor(true);
		itemMenu_->setShowCursor(true);
		itemMenu_->reset();
		break;
	}
}

void GameEquipMenu::setDiscriptionMessage()
{
	const rpg2k::model::DataBase& ldb = gameField_->getGameSystem().getLDB();
	descriptionWindow_->clearMessages();
	int itemID = 0;
	switch (state_) {
	case kStateEquip:
		itemID = charaStatus_->getEquip()[equipMenu_->cursor()];
		break;
	case kStateItem:
		itemID = itemList_.empty() ? 0 : itemList_[itemMenu_->cursor()];
		break;
	}
	if( itemID ) descriptionWindow_->addLine(ldb.item()[itemID][2].get_string());
}

void GameEquipMenu::updateItemWindow()
{
	const rpg2k::model::DataBase& ldb = gameField_->getGameSystem().getLDB();
	rpg2k::model::Project& sys = gameField_->getGameSystem();
	itemList_.clear();
	itemMenu_->clearMessages();
	char temp[256];
	for (uint i = 0; i < ldb.item().rbegin().first() + 1; i++) {
		if (sys.getLSD().getItemNum(i) > 0) {
			itemList_.push_back(i);
			sprintf(temp, " :%2d", sys.getLSD().getItemNum(i));
			itemMenu_->addLine(ldb.item()[i][1].get_string() + temp);
		}
	}
	itemList_.push_back(0);		// push empty. it is a symbol of unequip.
	itemMenu_->addLine("");
}

void GameEquipMenu::start()
{
	freeze(false);
	setState(kStateEquip);
}

void GameEquipMenu::draw()
{
	kuto::RenderManager::instance()->addRender(this, kuto::LAYER_2D_OBJECT, 20.f);
}

void GameEquipMenu::render()
{
}
