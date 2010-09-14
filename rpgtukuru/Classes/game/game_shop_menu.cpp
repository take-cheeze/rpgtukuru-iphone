/**
 * @file
 * @brief ショップ
 * @author project.kuto
 */

#include <sstream>
#include "game_shop_menu.h"
#include "game_system.h"
#include "game_select_window.h"
#include "game_message_window.h"
#include "game_inventory.h"


GameShopMenu::GameShopMenu(rpg2k::model::Project& gameSystem)
: kuto::Task()
, gameSystem_(gameSystem), state_(kStateSelectBuyOrSell), buyOrSell_(false), messageType_(0)
, checkItem_(0)
{
	buySellSelectWindow_ = addChild(GameSelectWindow::createTask(gameSystem));
	buySellSelectWindow_->pauseUpdate(true);
	buySellSelectWindow_->setPosition(kuto::Vector2(0.f, 160.f));
	buySellSelectWindow_->setSize(kuto::Vector2(320.f, 80.f));
	buySellSelectWindow_->setAutoClose(false);

	itemSelectWindow_ = addChild(GameSelectWindow::createTask(gameSystem));
	itemSelectWindow_->pauseUpdate(true);
	itemSelectWindow_->setPosition(kuto::Vector2(0.f, 32.f));
	itemSelectWindow_->setSize(kuto::Vector2(320.f, 128.f));
	itemSelectWindow_->setAutoClose(false);
	itemSelectWindow_->setColumnSize(2);
	itemSelectWindow_->setPauseUpdateCursor(true);
	itemSelectWindow_->setShowCursor(false);

	descriptionWindow_ = addChild(GameMessageWindow::createTask(gameSystem));
	descriptionWindow_->pauseUpdate(true);
	descriptionWindow_->setPosition(kuto::Vector2(0.f, 0.f));
	descriptionWindow_->setSize(kuto::Vector2(320.f, 32.f));
	descriptionWindow_->setEnableClick(false);
	descriptionWindow_->setUseAnimation(false);

	charaWindow_ = addChild(GameMessageWindow::createTask(gameSystem));
	charaWindow_->pauseUpdate(true);
	charaWindow_->setPosition(kuto::Vector2(180.f, 32.f));
	charaWindow_->setSize(kuto::Vector2(140.f, 48.f));
	charaWindow_->setEnableClick(false);
	charaWindow_->setUseAnimation(false);

	inventoryWindow_ = addChild(GameMessageWindow::createTask(gameSystem));
	inventoryWindow_->pauseUpdate(true);
	inventoryWindow_->setPosition(kuto::Vector2(180.f, 80.f));
	inventoryWindow_->setSize(kuto::Vector2(140.f, 48.f));
	inventoryWindow_->setEnableClick(false);
	inventoryWindow_->setUseAnimation(false);

	moneyWindow_ = addChild(GameMessageWindow::createTask(gameSystem));
	moneyWindow_->pauseUpdate(true);
	moneyWindow_->setPosition(kuto::Vector2(180.f, 128.f));
	moneyWindow_->setSize(kuto::Vector2(140.f, 32.f));
	moneyWindow_->setEnableClick(false);
	moneyWindow_->setUseAnimation(false);
	moneyWindow_->setMessageAlign(GameWindow::kAlignRight);
}

bool GameShopMenu::initialize()
{
	if (isInitializedChildren()) {
		buySellSelectWindow_->pauseUpdate(false);
		itemSelectWindow_->pauseUpdate(false);
		descriptionWindow_->pauseUpdate(false);
		charaWindow_->pauseUpdate(false);
		charaWindow_->freeze(true);
		inventoryWindow_->pauseUpdate(false);
		inventoryWindow_->freeze(true);
		moneyWindow_->pauseUpdate(false);
		moneyWindow_->freeze(true);
		return true;
	}
	return false;
}

void GameShopMenu::setShopData(int shopType, int mesType, const std::vector<int>& items)
{
	shopType_ = shopType;
	messageType_ = mesType;
	shopItems_ = items;
	setState(kStateSelectBuyOrSell);
}

void GameShopMenu::setState(State newState, bool thanks)
{
	State oldState = state_;
	state_ = newState;
	const rpg2k::model::DataBase& ldb = gameSystem_.getLDB();
	int vocBase = 41 + 13 * messageType_;
	rpg2k::model::SaveData& lsd = gameSystem_.getLSD();
	std::ostringstream oss;

	switch (state_) {
	case kStateSelectBuyOrSell:
		buySellSelectWindow_->reset();
		buySellSelectWindow_->resetCursor();
		buySellSelectWindow_->setPauseUpdateCursor(false);
		buySellSelectWindow_->setShowCursor(true);

		itemSelectWindow_->clearMessages();
		itemSelectWindow_->reset();
		itemSelectWindow_->resetCursor();
		itemSelectWindow_->setPauseUpdateCursor(true);
		itemSelectWindow_->setShowCursor(false);
		itemSelectWindow_->setSize(kuto::Vector2(320.f, 128.f));
		itemSelectWindow_->setColumnSize(2);

		charaWindow_->freeze(true);
		inventoryWindow_->freeze(true);
		moneyWindow_->freeze(true);

		buySellSelectWindow_->clearMessages();
		buySellSelectWindow_->addLine(ldb.vocabulary(vocBase + thanks));
		buySellSelectWindow_->addLine(ldb.vocabulary(vocBase + 2), shopType_ == 0 || shopType_ == 1);
		buySellSelectWindow_->addLine(ldb.vocabulary(vocBase + 3), shopType_ == 0 || shopType_ == 2);
		buySellSelectWindow_->addLine(ldb.vocabulary(vocBase + 4));
		buySellSelectWindow_->setCursorStart(1);
		buySellSelectWindow_->setEnableCancel(true);
		break;
	case kStateBuyItem:
		buySellSelectWindow_->clearMessages();
		buySellSelectWindow_->setPauseUpdateCursor(true);
		buySellSelectWindow_->setShowCursor(false);
		buySellSelectWindow_->addLine(ldb.vocabulary(vocBase + (thanks? 7 : 5)));

		itemSelectWindow_->clearMessages();
		itemSelectWindow_->reset();
		if (oldState != kStateBuyItemNum)
			itemSelectWindow_->resetCursor();
		itemSelectWindow_->setPauseUpdateCursor(false);
		itemSelectWindow_->setShowCursor(true);
		itemSelectWindow_->setSize(kuto::Vector2(180.f, 128.f));
		itemSelectWindow_->setColumnSize(1);
		for (unsigned int i = 0; i < shopItems_.size(); i++) {
			oss.str("");
			oss << ldb.item()[shopItems_[i]][1].get_string().toSystem() << " : " << ldb.item()[shopItems_[i]][5].get<int>();
			itemSelectWindow_->addLine(oss.str(), ldb.item()[shopItems_[i]][5].get<int>() <= lsd.getMoney());
		}

		charaWindow_->freeze(false);
		inventoryWindow_->freeze(false);
		moneyWindow_->freeze(false);
		moneyWindow_->clearMessages();
		oss.str("");
		oss << lsd.getMoney() << ldb.vocabulary(95).toSystem();
		moneyWindow_->addLine(oss.str());
		break;
	case kStateSellItem:
		buySellSelectWindow_->clearMessages();
		buySellSelectWindow_->setPauseUpdateCursor(true);
		buySellSelectWindow_->setShowCursor(false);
		buySellSelectWindow_->addLine(ldb.vocabulary(vocBase + (thanks? 10 : 8)));

		itemSelectWindow_->clearMessages();
		itemSelectWindow_->reset();
		itemSelectWindow_->resetCursor();
		itemSelectWindow_->setPauseUpdateCursor(false);
		itemSelectWindow_->setShowCursor(true);
		itemSelectWindow_->setSize(kuto::Vector2(320.f, 128.f));
		itemSelectWindow_->setColumnSize(2);
		sellItems_.clear();
		{
			rpg2k::structure::Array2D const& itemList = ldb.item();
			for (rpg2k::structure::Array2D::Iterator it = itemList.begin(); it != itemList.end(); ++it) {
				int itemNum = lsd.getItemNum(it.first());
				if (itemNum > 0) {
					oss.str("");
					oss << it.second()[1].get_string().toSystem() << " : " << itemNum;
					itemSelectWindow_->addLine(oss.str(), it.second()[5].get<int>() > 0);
					sellItems_.push_back(it.first());
				}
			}
		}

		charaWindow_->freeze(true);
		inventoryWindow_->freeze(true);
		moneyWindow_->freeze(true);
		break;
	case kStateBuyItemNum:
		buySellSelectWindow_->clearMessages();
		buySellSelectWindow_->addLine(ldb.vocabulary(vocBase + 6));

		itemSelectWindow_->setPauseUpdateCursor(false);
		itemSelectWindow_->setShowCursor(true);
		break;
	case kStateSellItemNum:
		buySellSelectWindow_->clearMessages();
		buySellSelectWindow_->addLine(ldb.vocabulary(vocBase + 9));

		itemSelectWindow_->setPauseUpdateCursor(false);
		itemSelectWindow_->setShowCursor(true);
		break;
	default: assert(false);
	}
}

void GameShopMenu::update()
{
	rpg2k::model::SaveData& lsd = gameSystem_.getLSD();
	const rpg2k::model::DataBase& ldb = gameSystem_.getLDB();
	updateDescriptionMessage();
	switch (state_) {
	case kStateSelectBuyOrSell:
		if (buySellSelectWindow_->selected() || buySellSelectWindow_->canceled()) {
			if (buySellSelectWindow_->canceled() || buySellSelectWindow_->cursor() == 3) {
				// おしまい
				setState(kStateClosed);
			} else {
				if (buySellSelectWindow_->cursor() == 1) {
					// 買う
					setState(kStateBuyItem);
				} else {
					// 売る
					setState(kStateSellItem);
				}
			}
		}
		break;
	case kStateBuyItem:
		if (itemSelectWindow_->canceled()) {
			setState(kStateSelectBuyOrSell, true);
		} else if (itemSelectWindow_->selected()) {
			if (!shopItems_.empty()) {
				checkItem_ = shopItems_[itemSelectWindow_->cursor()];
				setState(kStateBuyItemNum);
			}
		}
		break;
	case kStateSellItem:
		if (itemSelectWindow_->canceled()) {
			setState(kStateSelectBuyOrSell, true);
		} else if (itemSelectWindow_->selected()) {
			if (!sellItems_.empty()) {
				checkItem_ = sellItems_[itemSelectWindow_->cursor()];
				setState(kStateSellItemNum);
			}
		}
		break;
	case kStateBuyItemNum:
		//if (buySellSelectWindow_->canceled()) {
		//	setState(kStateBuyItem);
		//} else if (buySellSelectWindow_->selected()) {
			// ひとまず100%購入に。。。
			lsd.addMoney(-ldb.item()[checkItem_][5].get<int>());
			lsd.addItemNum(checkItem_, 1);
			setState(kStateBuyItem, true);
		//}
		break;
	case kStateSellItemNum:
		//if (buySellSelectWindow_->canceled()) {
		//	setState(kStateBuyItem);
		//} else if (buySellSelectWindow_->selected()) {
			// ひとまず100%売却に。。。
			lsd.addMoney(ldb.item()[checkItem_][5].get<int>() / 2);
			lsd.addItemNum(checkItem_, -1);
			setState(kStateSellItem, true);
		//}
		break;
	default: assert(false);
	}
}

void GameShopMenu::updateDescriptionMessage()
{
	const rpg2k::model::DataBase& ldb = gameSystem_.getLDB();
	rpg2k::model::SaveData& lsd = gameSystem_.getLSD();
	std::ostringstream oss;

	descriptionWindow_->clearMessages();
	switch (state_) {
	case kStateBuyItem:
		if (!shopItems_.empty()) {
			int itemID = shopItems_[itemSelectWindow_->cursor()];
			descriptionWindow_->addLine(ldb.item()[itemID][2].get_string().toSystem());
			inventoryWindow_->clearMessages();
			oss.str(""); oss << ldb.vocabulary(92).toSystem() << " : " << lsd.getItemNum(itemID);
			inventoryWindow_->addLine(oss.str());
			oss.str(""); oss << ldb.vocabulary(93).toSystem() << " : " << lsd.getEquipNum(itemID);
			inventoryWindow_->addLine(oss.str());
		}
		break;
	case kStateSellItem:
		if (!sellItems_.empty()) {
			descriptionWindow_->addLine(ldb.item()[sellItems_[itemSelectWindow_->cursor()]][2].get_string().toSystem());
		}
		break;
	case kStateBuyItemNum:
	case kStateSellItemNum:
		descriptionWindow_->addLine(ldb.item()[checkItem_][2].get_string().toSystem());
		inventoryWindow_->clearMessages();
		oss.str(""); oss << ldb.vocabulary(92).toSystem() << " : " << lsd.getItemNum(checkItem_);
		inventoryWindow_->addLine(oss.str());
		oss.str(""); oss << ldb.vocabulary(93).toSystem() << " : " << lsd.getEquipNum(checkItem_);
		inventoryWindow_->addLine(oss.str());
		break;
	default: assert(false);
	}
}
