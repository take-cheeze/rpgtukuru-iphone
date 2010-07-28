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


GameShopMenu::GameShopMenu(kuto::Task* parent, GameSystem& gameSystem)
: kuto::Task(parent)
, gameSystem_(gameSystem), state_(kStateSelectBuyOrSell), buyOrSell_(false), messageType_(0)
, checkItem_(0)
{
	buySellSelectWindow_ = GameSelectWindow::createTask(this, gameSystem);
	buySellSelectWindow_->pauseUpdate(true);
	buySellSelectWindow_->setPosition(kuto::Vector2(0.f, 160.f));
	buySellSelectWindow_->setSize(kuto::Vector2(320.f, 80.f));
	buySellSelectWindow_->setAutoClose(false);

	itemSelectWindow_ = GameSelectWindow::createTask(this, gameSystem);
	itemSelectWindow_->pauseUpdate(true);
	itemSelectWindow_->setPosition(kuto::Vector2(0.f, 32.f));
	itemSelectWindow_->setSize(kuto::Vector2(320.f, 128.f));
	itemSelectWindow_->setAutoClose(false);
	itemSelectWindow_->setColumnSize(2);
	itemSelectWindow_->setPauseUpdateCursor(true);
	itemSelectWindow_->setShowCursor(false);

	descriptionWindow_ = GameMessageWindow::createTask(this, gameSystem);
	descriptionWindow_->pauseUpdate(true);
	descriptionWindow_->setPosition(kuto::Vector2(0.f, 0.f));
	descriptionWindow_->setSize(kuto::Vector2(320.f, 32.f));
	descriptionWindow_->setEnableClick(false);
	descriptionWindow_->setUseAnimation(false);

	charaWindow_ = GameMessageWindow::createTask(this, gameSystem);
	charaWindow_->pauseUpdate(true);
	charaWindow_->setPosition(kuto::Vector2(180.f, 32.f));
	charaWindow_->setSize(kuto::Vector2(140.f, 48.f));
	charaWindow_->setEnableClick(false);
	charaWindow_->setUseAnimation(false);

	inventoryWindow_ = GameMessageWindow::createTask(this, gameSystem);
	inventoryWindow_->pauseUpdate(true);
	inventoryWindow_->setPosition(kuto::Vector2(180.f, 80.f));
	inventoryWindow_->setSize(kuto::Vector2(140.f, 48.f));
	inventoryWindow_->setEnableClick(false);
	inventoryWindow_->setUseAnimation(false);

	moneyWindow_ = GameMessageWindow::createTask(this, gameSystem);
	moneyWindow_->pauseUpdate(true);
	moneyWindow_->setPosition(kuto::Vector2(180.f, 128.f));
	moneyWindow_->setSize(kuto::Vector2(140.f, 32.f));
	moneyWindow_->setEnableClick(false);
	moneyWindow_->setUseAnimation(false);
	moneyWindow_->setMessageAlign(GameWindow::kAlignRight);
}

GameShopMenu::~GameShopMenu()
{
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
	const CRpgLdb& ldb = gameSystem_.getRpgLdb();
	const CRpgLdb::ShopTerm& shopTerm = ldb.term.shop[messageType_];
	GameInventory* inventory = gameSystem_.getInventory();
	
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
		buySellSelectWindow_->addMessage(thanks? shopTerm.what2 : shopTerm.what);
		buySellSelectWindow_->addMessage(shopTerm.buy, shopType_ == 0 || shopType_ == 1);
		buySellSelectWindow_->addMessage(shopTerm.sell, shopType_ == 0 || shopType_ == 2);
		buySellSelectWindow_->addMessage(shopTerm.cancel);
		buySellSelectWindow_->setCursorStart(1);
		buySellSelectWindow_->setEnableCancel(true);
		break;
	case kStateBuyItem:
		buySellSelectWindow_->clearMessages();
		buySellSelectWindow_->setPauseUpdateCursor(true);
		buySellSelectWindow_->setShowCursor(false);
		buySellSelectWindow_->addMessage(thanks? shopTerm.buyEnd : shopTerm.buyItem);
		
		itemSelectWindow_->clearMessages();
		itemSelectWindow_->reset();
		if (oldState != kStateBuyItemNum)
			itemSelectWindow_->resetCursor();
		itemSelectWindow_->setPauseUpdateCursor(false);
		itemSelectWindow_->setShowCursor(true);
		itemSelectWindow_->setSize(kuto::Vector2(180.f, 128.f));
		itemSelectWindow_->setColumnSize(1);
		for (unsigned int i = 0; i < shopItems_.size(); i++) {
			std::string mes = ldb.saItem[shopItems_[i]].name;
			std::ostringstream oss;
			oss << " : " << ldb.saItem[shopItems_[i]].price;
			mes += oss.str();
			itemSelectWindow_->addMessage(mes, ldb.saItem[shopItems_[i]].price <= inventory->getMoney());
		}
		
		charaWindow_->freeze(false);
		inventoryWindow_->freeze(false);
		moneyWindow_->freeze(false);
		moneyWindow_->clearMessages();
		{
			std::ostringstream oss;
			oss << inventory->getMoney() << ldb.term.shopParam.money;
			moneyWindow_->addMessage(oss.str());
		}
		break;
	case kStateSellItem:
		buySellSelectWindow_->clearMessages();
		buySellSelectWindow_->setPauseUpdateCursor(true);
		buySellSelectWindow_->setShowCursor(false);
		buySellSelectWindow_->addMessage(thanks? shopTerm.sellEnd : shopTerm.sellItem);
		
		itemSelectWindow_->clearMessages();
		itemSelectWindow_->reset();
		itemSelectWindow_->resetCursor();
		itemSelectWindow_->setPauseUpdateCursor(false);
		itemSelectWindow_->setShowCursor(true);
		itemSelectWindow_->setSize(kuto::Vector2(320.f, 128.f));
		itemSelectWindow_->setColumnSize(2);
		sellItems_.clear();
		for (unsigned int i = 1; i < ldb.saItem.GetSize(); i++) {
			int itemNum = inventory->getItemNum(i);
			if (itemNum > 0) {
				std::ostringstream oss;
				oss << ldb.saItem[i].name << " : " << itemNum;
				itemSelectWindow_->addMessage(oss.str(), ldb.saItem[i].price > 0);
				sellItems_.push_back(i);
			}
		}
		
		charaWindow_->freeze(true);
		inventoryWindow_->freeze(true);
		moneyWindow_->freeze(true);
		break;
	case kStateBuyItemNum:
		buySellSelectWindow_->clearMessages();
		buySellSelectWindow_->addMessage(shopTerm.buyNum);
		
		itemSelectWindow_->setPauseUpdateCursor(false);
		itemSelectWindow_->setShowCursor(true);
		break;
	case kStateSellItemNum:
		buySellSelectWindow_->clearMessages();
		buySellSelectWindow_->addMessage(shopTerm.sellNum);
		
		itemSelectWindow_->setPauseUpdateCursor(false);
		itemSelectWindow_->setShowCursor(true);
		break;
	default: assert(false);
	}
}

void GameShopMenu::update()
{
	GameInventory* inventory = gameSystem_.getInventory();
	const CRpgLdb& ldb = gameSystem_.getRpgLdb();
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
			inventory->addMoney(-ldb.saItem[checkItem_].price);
			inventory->addItemNum(checkItem_, 1);
			setState(kStateBuyItem, true);
		//}
		break;
	case kStateSellItemNum:
		//if (buySellSelectWindow_->canceled()) {
		//	setState(kStateBuyItem);
		//} else if (buySellSelectWindow_->selected()) {
			// ひとまず100%売却に。。。
			inventory->addMoney(ldb.saItem[checkItem_].price / 2);
			inventory->addItemNum(checkItem_, -1);
			setState(kStateSellItem, true);
		//}
		break;
	default: assert(false);
	}
}

void GameShopMenu::updateDescriptionMessage()
{
	const CRpgLdb& ldb = gameSystem_.getRpgLdb();
	GameInventory* inventory = gameSystem_.getInventory();

	descriptionWindow_->clearMessages();
	switch (state_) {
	case kStateBuyItem:
		if (!shopItems_.empty()) {
			descriptionWindow_->addMessage(ldb.saItem[shopItems_[itemSelectWindow_->cursor()]].explain);
			inventoryWindow_->clearMessages();
			{
				std::ostringstream oss;
				oss << ldb.term.shopParam.itemGet << " : " << inventory->getItemNum(shopItems_[itemSelectWindow_->cursor()]);
				inventoryWindow_->addMessage(oss.str());
				oss.str("");
				oss << ldb.term.shopParam.itemEquiped << " : " << 0;	// Undefined
				inventoryWindow_->addMessage(oss.str());
			}
		}
		break;
	case kStateSellItem:
		if (!sellItems_.empty()) {
			descriptionWindow_->addMessage(ldb.saItem[sellItems_[itemSelectWindow_->cursor()]].explain);
		}
		break;
	case kStateBuyItemNum:
	case kStateSellItemNum:
		descriptionWindow_->addMessage(ldb.saItem[checkItem_].explain);
		inventoryWindow_->clearMessages();
		{
			std::ostringstream oss;
			oss << ldb.term.shopParam.itemGet << " : " << inventory->getItemNum(checkItem_);
			inventoryWindow_->addMessage(oss.str());
			oss.str("");
			oss << ldb.term.shopParam.itemEquiped << " : " << 0;	// Undefined
			inventoryWindow_->addMessage(oss.str());
		}
		break;
	default: assert(false);
	}
}
