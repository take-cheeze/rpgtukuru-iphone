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

using namespace rpg2kLib;

GameEquipMenu::GameEquipMenu(GameField* gameField, GameCharaStatus* charaStatus)
: GameSystemMenuBase(gameField)
, charaStatus_(charaStatus)
{
	const DataBase& ldb = gameField_->getGameSystem().getRpgLdb();
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
	statusWindow_->addMessage(ldb.getCharacter()[charaStatus_->getCharaId()][1]);

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
			Equip equip = charaStatus_->getEquip();
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
	const DataBase& ldb = gameField_->getGameSystem().getRpgLdb();
	switch (state_) {
	case kStateEquip:
		{
			itemMenu_->setPauseUpdateCursor(true);
			itemMenu_->setShowCursor(false);
			equipMenu_->clearMessages();

			const Equip& equip = charaStatus_->getEquip();
			const Array1D& player = ldb.getCharacter()[charaStatus_->getCharaId()];
			const Array1D& voc = ldb.getVocabulary();
			const Array2D& itemList = ldb.getItem();

			vector< string > typeName, equipName;
			typeName.resize(EQUIP_NUM);
			equipName.resize(EQUIP_NUM);

			for(int i = 0; i < EQUIP_NUM; i++) {
				typeName[i] = static_cast< string >(voc[0x88+i]);
				if(equip[i])
					equipName[i] = static_cast< string >(itemList[ equip[i] ][1]);
			}
			if( static_cast< bool >(player[21]) )
				typeName[1] = static_cast< string >(voc[0x88]);

			for(int i = 0; i < EQUIP_NUM; i++)
				equipMenu_->addMessage(typeName[i] + " " + equipName[i]);

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
	const DataBase& ldb = gameField_->getGameSystem().getRpgLdb();
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
				descriptionWindow_->addMessage(ldb.getItem()[itemId][2]);
		}
		break;
	case kStateItem:
		if (!itemList_.empty() && itemList_[itemMenu_->cursor()] > 0)
			descriptionWindow_->addMessage(ldb.getItem()[itemList_[itemMenu_->cursor()]][2]);
		break;
	}
}

void GameEquipMenu::updateItemWindow()
{
	const DataBase& ldb = gameField_->getGameSystem().getRpgLdb();
	GameInventory* inventory = gameField_->getGameSystem().getInventory();
	itemList_.clear();
	itemMenu_->clearMessages();
	char temp[256];
	for (u32 i = 0; i < inventory->getItemList().size(); i++) {
		if (inventory->getItemNum(i) > 0) {
			itemList_.push_back(i);
			sprintf(temp, "%s :%2d", static_cast< string& >(ldb.getItem()[i][1]).c_str(), inventory->getItemNum(i));
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
