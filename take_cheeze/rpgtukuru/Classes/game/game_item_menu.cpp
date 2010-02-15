/**
 * @file
 * @brief Save Menu
 * @author project.kuto
 */

#include <sstream>

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

using namespace std;

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
	const DataBase& ldb = gameField_->getGameSystem().getRpgLdb();
	switch (state_) {
	case kStateItem:
		updateDiscriptionMessage();
		if (itemMenu_->selected()) {
			GameInventory* inventory = gameField_->getGameSystem().getInventory();
			const Array1D& item = ldb.getItem()[itemList_[itemMenu_->cursor()]];
			switch ( static_cast< int >(item[3]) ) {
			case DataBase::kItemTypeMedicine:
			case DataBase::kItemTypeBook:
			case DataBase::kItemTypeSeed:
				setState(kStateChara);
				break;
			case DataBase::kItemTypeSwitch:
				if ( static_cast< bool >(item[57]) ) {
					if ( static_cast< int >(item[6]) > 0 )
						inventory->addItemNum(itemList_[itemMenu_->cursor()], -1);		// 使用回数は無限のみ対応
					gameField_->getGameSystem().setSwitch(item[55], true);
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
			const Array1D& item = ldb.getItem()[itemList_[itemMenu_->cursor()]];
			if (inventory->getItemNum(itemList_[itemMenu_->cursor()]) > 0) {
				int playerId = ( static_cast< int >(item[31]) == DataBase::kItemScopeSingle)? gameField_->getPlayers()[charaMenu_->cursor()]->getPlayerId() : 0;
				if (applyItem(itemList_[itemMenu_->cursor()], playerId)) {
					if ( static_cast< int >(item[6]) > 0)
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
	const DataBase& ldb = gameField_->getGameSystem().getRpgLdb();

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
			const Array1D& item = ldb.getItem()[itemList_[itemMenu_->cursor()]];
			charaMenu_->setFullSelect( static_cast< int >(item[31]) != DataBase::kItemScopeSingle);
		}
		break;
	}
}

void GameItemMenu::updateDiscriptionMessage()
{
	const DataBase& ldb = gameField_->getGameSystem().getRpgLdb();

	descriptionWindow_->clearMessages();
	itemNameWindow_->clearMessages();
	itemNumWindow_->clearMessages();

	switch (state_) {
	case kStateItem:
		if (!itemList_.empty() && itemList_[itemMenu_->cursor()] > 0)
			descriptionWindow_->addMessage(ldb.getItem()[itemList_[itemMenu_->cursor()]][2]);
		break;
	case kStateChara:
		if (!itemList_.empty() && itemList_[itemMenu_->cursor()] > 0) {
			itemNameWindow_->addMessage(ldb.getItem()[itemList_[itemMenu_->cursor()]][1]);

			string message;
			ostringstream strm(message);

			strm << static_cast< string& >(ldb.getVocabulary()[0x5c]) << " " <<
				gameField_->getGameSystem().getInventory()->getItemNum(itemList_[itemMenu_->cursor()]);

			itemNumWindow_->addMessage(message);
		}
		break;
	default: break;
	}
}

void GameItemMenu::updateItemWindow()
{
	const DataBase& ldb = gameField_->getGameSystem().getRpgLdb();
	GameInventory* inventory = gameField_->getGameSystem().getInventory();
	itemList_.clear();
	itemMenu_->clearMessages();

	string message;
	ostringstream strm(message);

	for (u32 i = 0; i < inventory->getItemList().size(); i++) {
		if (inventory->getItemNum(i) > 0) {
			itemList_.push_back(i);

			strm  << static_cast< string& >(ldb.getItem()[i][1]) << " : ";
			strm.width(); strm << inventory->getItemNum(i);

			itemMenu_->addMessage(message);
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
