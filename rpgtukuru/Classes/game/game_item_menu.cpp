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
	itemMenu_ = addChild(GameSelectWindow::createTask(gameField_->getGameSystem()));
	itemMenu_->pauseUpdate(true);
	itemMenu_->setPosition(kuto::Vector2(0.f, 32.f));
	itemMenu_->setSize(kuto::Vector2(320.f, 208.f));
	itemMenu_->setAutoClose(false);
	itemMenu_->setColumnSize(2);

	descriptionWindow_ = addChild(GameMessageWindow::createTask(gameField_->getGameSystem()));
	descriptionWindow_->pauseUpdate(true);
	descriptionWindow_->setPosition(kuto::Vector2(0.f, 0.f));
	descriptionWindow_->setSize(kuto::Vector2(320.f, 32.f));
	descriptionWindow_->setEnableClick(false);
	descriptionWindow_->setUseAnimation(false);

	charaMenu_ = addChild(kuto::TaskCreatorParam1<GameCharaSelectMenu, GameField*>::createTask(gameField_));
	charaMenu_->pauseUpdate(true);
	charaMenu_->setPosition(kuto::Vector2(136.f, 0.f));
	charaMenu_->setSize(kuto::Vector2(184.f, 240.f));
	charaMenu_->setAutoClose(false);

	itemNameWindow_ = addChild(GameMessageWindow::createTask(gameField_->getGameSystem()));
	itemNameWindow_->pauseUpdate(true);
	itemNameWindow_->setPosition(kuto::Vector2(0.f, 0.f));
	itemNameWindow_->setSize(kuto::Vector2(136.f, 32.f));
	itemNameWindow_->setEnableClick(false);
	itemNameWindow_->setUseAnimation(false);

	itemNumWindow_ = addChild(GameMessageWindow::createTask(gameField_->getGameSystem()));
	itemNumWindow_->pauseUpdate(true);
	itemNumWindow_->setPosition(kuto::Vector2(0.f, 32.f));
	itemNumWindow_->setSize(kuto::Vector2(136.f, 32.f));
	itemNumWindow_->setEnableClick(false);
	itemNumWindow_->setUseAnimation(false);
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
	const rpg2k::model::DataBase& ldb = gameField_->getGameSystem().getLDB();
	rpg2k::model::SaveData& lsd = gameField_->getGameSystem().getLSD();
	const rpg2k::structure::Array1D& item = ldb.item()[itemList_[itemMenu_->cursor()]];
	switch (state_) {
	case kStateItem:
		updateDiscriptionMessage();
		if (itemMenu_->selected()) {
			switch (item[3].get<int>()) {
			case rpg2k::Item::MEDICINE:
			case rpg2k::Item::BOOK:
			case rpg2k::Item::SEED:
				setState(kStateChara);
				break;
			case rpg2k::Item::SWITCH:
				if (item[57].get<bool>()) {
					if (item[6].get<int>() > 0)
						lsd.addItemNum(itemList_[itemMenu_->cursor()], -1);		// 使用回数は無限のみ対応
					lsd.setFlag(item[55].get<int>(), true);
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
			if (lsd.getItemNum(itemList_[itemMenu_->cursor()]) > 0) {
				int playerId = (item[31].get<int>() == 0)? gameField_->getPlayers()[charaMenu_->cursor()]->getPlayerId() : 0;
				if (applyItem(itemList_[itemMenu_->cursor()], playerId)) {
					if (item[6].get<int>() > 0)
						lsd.addItemNum(itemList_[itemMenu_->cursor()], -1);		// 使用回数は無限のみ対応
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
		for (uint i = 0; i < gameField_->getPlayers().size(); i++)
			statusList.push_back(&gameField_->getPlayers()[i]->getStatus());
	} else {
		statusList.push_back(&gameField_->getPlayerFromId(playerId)->getStatus());
	}
	bool applyOk = false;
	for (uint i = 0; i < statusList.size(); i++) {
		if (statusList[i]->applyItem(itemId)) {
			applyOk = true;
		}
	}
	return applyOk;
}

void GameItemMenu::setState(int newState)
{
	state_ = newState;
	const rpg2k::model::DataBase& ldb = gameField_->getGameSystem().getLDB();
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
			const rpg2k::structure::Array1D& item = ldb.item()[itemList_[itemMenu_->cursor()]];
			charaMenu_->setFullSelect(item[31]);
		}
		break;
	}
}

void GameItemMenu::updateDiscriptionMessage()
{
	const rpg2k::model::DataBase& ldb = gameField_->getGameSystem().getLDB();
	descriptionWindow_->clearMessages();
	itemNameWindow_->clearMessages();
	itemNumWindow_->clearMessages();
	switch (state_) {
	case kStateItem:
		if (!itemList_.empty() && itemList_[itemMenu_->cursor()] > 0)
			descriptionWindow_->addLine(ldb.item()[itemList_[itemMenu_->cursor()]][2].get_string().toSystem());
		break;
	case kStateChara:
		if (!itemList_.empty() && itemList_[itemMenu_->cursor()] > 0) {
			itemNameWindow_->addLine(ldb.item()[itemList_[itemMenu_->cursor()]][1].get_string().toSystem());
			char temp[256];
			sprintf(temp, "%s %d", ldb[10].get_string().toSystem().c_str(),
				gameField_->getGameSystem().getLSD().getItemNum(itemList_[itemMenu_->cursor()]));
			itemNumWindow_->addLine(temp);
		}
		break;
	}
}

void GameItemMenu::updateItemWindow()
{
	const rpg2k::model::DataBase& ldb = gameField_->getGameSystem().getLDB();
	rpg2k::model::SaveData& lsd = gameField_->getGameSystem().getLSD();
	itemList_.clear();
	itemMenu_->clearMessages();
	char temp[256];
	for(rpg2k::structure::Array2D::Iterator it = ldb.item().begin(); it != ldb.item().end(); ++it) {
		if (lsd.getItemNum(it.first()) > 0) {
			itemList_.push_back(it.first());
			sprintf(temp, "%s : %2d", it.second()[1].get_string().toSystem().c_str(), lsd.getItemNum(it.first()));
			itemMenu_->addLine(temp);
		}
	}
}

void GameItemMenu::start()
{
	freeze(false);
	setState(kStateItem);
}

void GameItemMenu::render(kuto::Graphics2D* g) const
{
}
