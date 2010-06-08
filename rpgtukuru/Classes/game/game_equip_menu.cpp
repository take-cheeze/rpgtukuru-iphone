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
	// const CRpgLdb& ldb = gameField_->getGameSystem().getRpgLdb();
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
	statusWindow_->addMessage(gameField_->getGameSystem().getPlayerInfo(charaStatus_->getCharaId()).name);

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
			CRpgLdb::Equip equip = charaStatus_->getEquip();
			int oldId = 0;
			switch (equipMenu_->cursor()) {
			case 0:
				oldId = equip.weapon;
				equip.weapon = itemList_[itemMenu_->cursor()];
				break;
			case 1:
				oldId = equip.shield;
				equip.shield = itemList_[itemMenu_->cursor()];
				break;
			case 2:
				oldId = equip.protector;
				equip.protector = itemList_[itemMenu_->cursor()];
				break;
			case 3:
				oldId = equip.helmet;
				equip.helmet = itemList_[itemMenu_->cursor()];
				break;
			case 4:
				oldId = equip.accessory;
				equip.accessory = itemList_[itemMenu_->cursor()];
				break;
			}
			charaStatus_->setEquip(equip);
			if (oldId > 0)
				gameField_->getGameSystem().getInventory()->addItemNum(oldId, 1);
			if (itemList_[itemMenu_->cursor()] > 0)
				gameField_->getGameSystem().getInventory()->addItemNum(itemList_[itemMenu_->cursor()], -1);
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
	const CRpgLdb& ldb = gameField_->getGameSystem().getRpgLdb();
	switch (state_) {
	case kStateEquip:
		{
			itemMenu_->setPauseUpdateCursor(true);
			itemMenu_->setShowCursor(false);
			equipMenu_->clearMessages();
			const CRpgLdb::Equip& equip = charaStatus_->getEquip();
			const CRpgLdb::Player& player = ldb.saPlayer[charaStatus_->getCharaId()];
			char temp[256];
			sprintf(temp, "%s %s", ldb.term.param.weapon.c_str(), equip.weapon > 0? ldb.saItem[equip.weapon].name.c_str() : "");
			equipMenu_->addMessage(temp);
			sprintf(temp, "%s %s", player.twinSword? ldb.term.param.weapon.c_str() : ldb.term.param.shield.c_str(),
				equip.shield > 0? ldb.saItem[equip.shield].name.c_str() : "");
			equipMenu_->addMessage(temp);
			sprintf(temp, "%s %s", ldb.term.param.protector.c_str(), equip.protector > 0? ldb.saItem[equip.protector].name.c_str() : "");
			equipMenu_->addMessage(temp);
			sprintf(temp, "%s %s", ldb.term.param.helmet.c_str(), equip.helmet > 0? ldb.saItem[equip.helmet].name.c_str() : "");
			equipMenu_->addMessage(temp);
			sprintf(temp, "%s %s", ldb.term.param.accessory.c_str(), equip.accessory > 0? ldb.saItem[equip.accessory].name.c_str() : "");
			equipMenu_->addMessage(temp);
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
	const CRpgLdb& ldb = gameField_->getGameSystem().getRpgLdb();
	descriptionWindow_->clearMessages();
	switch (state_) {
	case kStateEquip:
		{
			int itemId = 0;
			switch (equipMenu_->cursor()) {
			case 0: itemId = charaStatus_->getEquip().weapon; break;
			case 1: itemId = charaStatus_->getEquip().shield; break;
			case 2: itemId = charaStatus_->getEquip().protector; break;
			case 3: itemId = charaStatus_->getEquip().helmet; break;
			case 4: itemId = charaStatus_->getEquip().accessory; break;
			}
			if (itemId > 0)
				descriptionWindow_->addMessage(ldb.saItem[itemId].explain);
		}
		break;
	case kStateItem:
		if (!itemList_.empty() && itemList_[itemMenu_->cursor()] > 0)
			descriptionWindow_->addMessage(ldb.saItem[itemList_[itemMenu_->cursor()]].explain);
		break;
	}
}

void GameEquipMenu::updateItemWindow()
{
	const CRpgLdb& ldb = gameField_->getGameSystem().getRpgLdb();
	GameInventory* inventory = gameField_->getGameSystem().getInventory();
	itemList_.clear();
	itemMenu_->clearMessages();
	char temp[256];
	for (unsigned int i = 0; i < inventory->getItemList().size(); i++) {
		if (inventory->getItemNum(i) > 0) {
			itemList_.push_back(i);
			sprintf(temp, "%s :%2d", ldb.saItem[i].name.c_str(), inventory->getItemNum(i));
			itemMenu_->addMessage(temp);
		}
	}
	itemList_.push_back(0);		// push empty. it is a symbol of unequip.
	itemMenu_->addMessage("");
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
