/**
 * @file
 * @brief Save Menu
 * @author project.kuto
 */

#include <kuto/kuto_render_manager.h>
#include <kuto/kuto_graphics2d.h>
#include "game_item_menu.h"
#include "game_system.h"
#include "game_field.h"
#include "game_select_window.h"
#include "game_message_window.h"
#include "game_chara_status.h"
#include "game_inventory.h"
#include "game_chara_select_menu.h"
#include "game_player.h"


GameItemMenu::GameItemMenu(GameField* gameField)
: GameSystemMenuBase(gameField)
{
	itemMenu_ = GameSelectWindow::createTask(this, gameField_->getGameSystem());
	itemMenu_->pauseUpdate(true);
	itemMenu_->setPosition(kuto::Vector2(0.f, 32.f));
	itemMenu_->setSize(kuto::Vector2(320.f, 208.f));
	itemMenu_->setAutoClose(false);
	itemMenu_->setColumnSize(2);

	descriptionWindow_ = GameMessageWindow::createTask(this, gameField_->getGameSystem());
	descriptionWindow_->pauseUpdate(true);
	descriptionWindow_->setPosition(kuto::Vector2(0.f, 0.f));
	descriptionWindow_->setSize(kuto::Vector2(320.f, 32.f));
	descriptionWindow_->setEnableClick(false);
	descriptionWindow_->setUseAnimation(false);
	
	charaMenu_ = GameCharaSelectMenu::createTask(this, gameField_);
	charaMenu_->pauseUpdate(true);
	charaMenu_->setPosition(kuto::Vector2(136.f, 0.f));
	charaMenu_->setSize(kuto::Vector2(184.f, 240.f));
	charaMenu_->setAutoClose(false);
	
	itemNameWindow_ = GameMessageWindow::createTask(this, gameField_->getGameSystem());
	itemNameWindow_->pauseUpdate(true);
	itemNameWindow_->setPosition(kuto::Vector2(0.f, 0.f));
	itemNameWindow_->setSize(kuto::Vector2(136.f, 32.f));
	itemNameWindow_->setEnableClick(false);
	itemNameWindow_->setUseAnimation(false);
	
	itemNumWindow_ = GameMessageWindow::createTask(this, gameField_->getGameSystem());
	itemNumWindow_->pauseUpdate(true);
	itemNumWindow_->setPosition(kuto::Vector2(0.f, 32.f));
	itemNumWindow_->setSize(kuto::Vector2(136.f, 32.f));
	itemNumWindow_->setEnableClick(false);
	itemNumWindow_->setUseAnimation(false);
}

GameItemMenu::~GameItemMenu()
{
}

bool GameItemMenu::initialize()
{
	if (isInitializedChildren()) {
		itemMenu_->pauseUpdate(false);
		descriptionWindow_->pauseUpdate(false);
		charaMenu_->pauseUpdate(false);
		charaMenu_->freeze(true);
		itemNameWindow_->pauseUpdate(false);
		itemNameWindow_->freeze(true);
		itemNumWindow_->pauseUpdate(false);
		itemNumWindow_->freeze(true);
		start();
		return true;
	}
	return false;
}

void GameItemMenu::update()
{
	const CRpgLdb& ldb = gameField_->getGameSystem().getRpgLdb();
	switch (state_) {
	case kStateItem:
		updateDiscriptionMessage();
		if (itemMenu_->selected()) {
			GameInventory* inventory = gameField_->getGameSystem().getInventory();
			const CRpgLdb::Item& item = ldb.saItem[itemList_[itemMenu_->cursor()]];
			switch (item.type) {
			case CRpgLdb::kItemTypeMedicine:
			case CRpgLdb::kItemTypeBook:
			case CRpgLdb::kItemTypeSeed:
				setState(kStateChara);
				break;
			case CRpgLdb::kItemTypeSwitch:
				if (item.useField) {
					if (item.useCount > 0)
						inventory->addItemNum(itemList_[itemMenu_->cursor()], -1);		// 使用回数は無限のみ対応
					gameField_->getGameSystem().setSwitch(item.onSwitch, true);
					setState(kStateSystemMenuEnd);
				} else {
					itemMenu_->reset();
				}
				break;
			default:
				itemMenu_->reset();
				break;
			}
		} else if (itemMenu_->canceled()) {
			setState(kStateNone);
		}
		break;
	case kStateChara:
		updateDiscriptionMessage();
		if (charaMenu_->selected()) {
			GameInventory* inventory = gameField_->getGameSystem().getInventory();
			const CRpgLdb::Item& item = ldb.saItem[itemList_[itemMenu_->cursor()]];
			if (inventory->getItemNum(itemList_[itemMenu_->cursor()]) > 0) {
				int playerId = (item.scope == CRpgLdb::kItemScopeSingle)? gameField_->getPlayers()[charaMenu_->cursor()]->getPlayerId() : 0;
				if (applyItem(itemList_[itemMenu_->cursor()], playerId)) {
					if (item.useCount > 0)
						inventory->addItemNum(itemList_[itemMenu_->cursor()], -1);		// 使用回数は無限のみ対応
				}
			}
			charaMenu_->reset();
		} else if (charaMenu_->canceled()) {
			setState(kStateItem);
		}
		break;
	}
}

bool GameItemMenu::applyItem(int itemId, int playerId)
{
	kuto::StaticVector<GameCharaStatus*, 4> statusList;
	if (playerId == 0) {
		// party
		for (u32 i = 0; i < gameField_->getPlayers().size(); i++)
			statusList.push_back(&gameField_->getPlayers()[i]->getStatus());
	} else {
		statusList.push_back(&gameField_->getPlayerFromId(playerId)->getStatus());
	}
	bool applyOk = false;
	for (u32 i = 0; i < statusList.size(); i++) {
		if (statusList[i]->applyItem(itemId)) {
			applyOk = true;
		}
	}
	return applyOk;
}

void GameItemMenu::setState(int newState)
{
	state_ = newState;
	const CRpgLdb& ldb = gameField_->getGameSystem().getRpgLdb();
	switch (state_) {
	case kStateItem:
		itemMenu_->freeze(false);
		descriptionWindow_->freeze(false);
		charaMenu_->freeze(true);
		itemNameWindow_->freeze(true);
		itemNumWindow_->freeze(true);
		updateItemWindow();
		itemMenu_->setPauseUpdateCursor(false);
		itemMenu_->setShowCursor(true);
		itemMenu_->reset();
		break;
	case kStateChara:
		itemMenu_->freeze(true);
		descriptionWindow_->freeze(true);
		charaMenu_->freeze(false);
		itemNameWindow_->freeze(false);
		itemNumWindow_->freeze(false);
		charaMenu_->reset();
		{
			const CRpgLdb::Item& item = ldb.saItem[itemList_[itemMenu_->cursor()]];
			charaMenu_->setFullSelect(item.scope != CRpgLdb::kItemScopeSingle);
		}
		break;
	}
}

void GameItemMenu::updateDiscriptionMessage()
{
	const CRpgLdb& ldb = gameField_->getGameSystem().getRpgLdb();
	descriptionWindow_->clearMessages();
	itemNameWindow_->clearMessages();
	itemNumWindow_->clearMessages();
	switch (state_) {
	case kStateItem:
		if (!itemList_.empty() && itemList_[itemMenu_->cursor()] > 0)
			descriptionWindow_->addMessage(ldb.saItem[itemList_[itemMenu_->cursor()]].explain);
		break;
	case kStateChara:
		if (!itemList_.empty() && itemList_[itemMenu_->cursor()] > 0) {
			itemNameWindow_->addMessage(ldb.saItem[itemList_[itemMenu_->cursor()]].name);
			char temp[256];
			sprintf(temp, "%s %d", ldb.term.shopParam.itemGet.c_str(),
				gameField_->getGameSystem().getInventory()->getItemNum(itemList_[itemMenu_->cursor()]));
			itemNumWindow_->addMessage(temp);
		}
		break;
	}
}

void GameItemMenu::updateItemWindow()
{
	const CRpgLdb& ldb = gameField_->getGameSystem().getRpgLdb();
	GameInventory* inventory = gameField_->getGameSystem().getInventory();
	itemList_.clear();
	itemMenu_->clearMessages();
	char temp[256];
	for (u32 i = 0; i < inventory->getItemList().size(); i++) {
		if (inventory->getItemNum(i) > 0) {
			itemList_.push_back(i);
			sprintf(temp, "%s : %2d", ldb.saItem[i].name.c_str(), inventory->getItemNum(i));
			itemMenu_->addMessage(temp);
		}
	}
}

void GameItemMenu::start()
{
	freeze(false);
	setState(kStateItem);
}

void GameItemMenu::draw()
{
	kuto::RenderManager::instance()->addRender(this, kuto::LAYER_2D_OBJECT, 20.f);
}

void GameItemMenu::render()
{
}
