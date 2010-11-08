/**
 * @file
 * @brief Save Menu
 * @author project.kuto
 */

#include <kuto/kuto_render_manager.h>
#include <kuto/kuto_graphics2d.h>
#include <rpg2k/Project.hpp>

#include "game_item_menu.h"
#include "game_field.h"
#include "game_select_window.h"
#include "game_message_window.h"
#include "game_chara_status.h"
#include "game_chara_select_menu.h"

#include <iterator>
#include <algorithm>


GameItemMenu::GameItemMenu(GameField& gameField)
: GameSystemMenuBase(gameField)
, itemMenu_( *addChild(GameSelectWindow::createTask(field_.game())) )
, descriptionWindow_( *addChild(GameMessageWindow::createTask(field_.game())) )
, charaMenu_( *addChild(kuto::TaskCreatorParam1<GameCharaSelectMenu, GameField&>::createTask(field_)) )
, itemNameWindow_( *addChild(GameMessageWindow::createTask(field_.game())) )
, itemNumWindow_( *addChild(GameMessageWindow::createTask(field_.game())) )
{
	itemMenu_.pauseUpdate();
	itemMenu_.setPosition(kuto::Vector2(0.f, 32.f));
	itemMenu_.setSize(kuto::Vector2(320.f, 208.f));
	itemMenu_.setAutoClose(false);
	itemMenu_.setColumnSize(2);

	descriptionWindow_.pauseUpdate();
	descriptionWindow_.setPosition(kuto::Vector2(0.f, 0.f));
	descriptionWindow_.setSize(kuto::Vector2(320.f, 32.f));
	descriptionWindow_.enableClick(false);
	descriptionWindow_.useAnimation(false);

	charaMenu_.pauseUpdate();
	charaMenu_.setPosition(kuto::Vector2(136.f, 0.f));
	charaMenu_.setSize(kuto::Vector2(184.f, 240.f));
	charaMenu_.setAutoClose(false);

	itemNameWindow_.pauseUpdate();
	itemNameWindow_.setPosition(kuto::Vector2(0.f, 0.f));
	itemNameWindow_.setSize(kuto::Vector2(136.f, 32.f));
	itemNameWindow_.enableClick(false);
	itemNameWindow_.useAnimation(false);

	itemNumWindow_.pauseUpdate();
	itemNumWindow_.setPosition(kuto::Vector2(0.f, 32.f));
	itemNumWindow_.setSize(kuto::Vector2(136.f, 32.f));
	itemNumWindow_.enableClick(false);
	itemNumWindow_.useAnimation(false);
}

bool GameItemMenu::initialize()
{
	if (isInitializedChildren()) {
		itemMenu_.pauseUpdate(false);
		descriptionWindow_.pauseUpdate(false);
		charaMenu_.pauseUpdate(false);
		charaMenu_.freeze();
		itemNameWindow_.pauseUpdate(false);
		itemNameWindow_.freeze();
		itemNumWindow_.pauseUpdate(false);
		itemNumWindow_.freeze();
		start();
		return true;
	}
	return false;
}

void GameItemMenu::update()
{
	const rpg2k::model::DataBase& ldb = field_.project().getLDB();
	rpg2k::model::SaveData& lsd = field_.project().getLSD();
	const rpg2k::structure::Array1D& item = ldb.item()[itemList_[itemMenu_.cursor()]];
	switch (state_) {
	case kStateItem:
		updateDiscriptionMessage();
		if (itemMenu_.selected()) {
			switch (item[3].to<int>()) {
			case rpg2k::Item::MEDICINE:
			case rpg2k::Item::BOOK:
			case rpg2k::Item::SEED:
				setState(kStateChara);
				break;
			case rpg2k::Item::SWITCH:
				if (item[57].to<bool>()) {
					if (item[6].to<int>() > 0)
						lsd.addItemNum(itemList_[itemMenu_.cursor()], -1);		// 使用回数は無限のみ対応
					lsd.setFlag(item[55].to<int>(), true);
					setState(kStateSystemMenuEnd);
				} else {
					itemMenu_.reset();
				}
				break;
			default:
				itemMenu_.reset();
				break;
			}
		} else if (itemMenu_.canceled()) {
			setState(kStateNone);
		}
		break;
	case kStateChara:
		updateDiscriptionMessage();
		if (charaMenu_.selected()) {
			if (lsd.itemNum(itemList_[itemMenu_.cursor()]) > 0) {
				int playerId = (item[31].to<int>() == 0)? field_.project().getLSD().member()[charaMenu_.cursor()] : 0;
				if (applyItem(itemList_[itemMenu_.cursor()], playerId)) {
					if (item[6].to<int>() > 0)
						lsd.addItemNum(itemList_[itemMenu_.cursor()], -1);		// 使用回数は無限のみ対応
				}
			}
			charaMenu_.reset();
		} else if (charaMenu_.canceled()) {
			setState(kStateItem);
		}
		break;
	}
}

bool GameItemMenu::applyItem(int const itemId, int const playerId)
{
	kuto::StaticVector<unsigned, rpg2k::MEMBER_MAX> target;
	if (playerId == 0) {
		// party
		std::vector<uint16_t> const& mem = field_.project().getLSD().member();
		std::copy( mem.begin(), mem.end(), std::back_inserter(target) );
	} else { target.push_back(playerId); }
	bool applyOk = false;
	for (uint i = 0; i < target.size(); i++) {
		/* TODO
		if (target[i]->applyItem(itemId)) {
			applyOk = true;
		} */
	}
	return applyOk;
}

void GameItemMenu::setState(int newState)
{
	state_ = newState;
	const rpg2k::model::DataBase& ldb = field_.project().getLDB();
	switch (state_) {
	case kStateItem:
		itemMenu_.freeze(false);
		descriptionWindow_.freeze(false);
		charaMenu_.freeze();
		itemNameWindow_.freeze();
		itemNumWindow_.freeze();
		updateItemWindow();
		itemMenu_.setPauseUpdateCursor(false);
		itemMenu_.setShowCursor(true);
		itemMenu_.reset();
		break;
	case kStateChara:
		itemMenu_.freeze();
		descriptionWindow_.freeze();
		charaMenu_.freeze(false);
		itemNameWindow_.freeze(false);
		itemNumWindow_.freeze(false);
		charaMenu_.reset();
		{
			const rpg2k::structure::Array1D& item = ldb.item()[itemList_[itemMenu_.cursor()]];
			charaMenu_.setFullSelect(item[31]);
		}
		break;
	}
}

void GameItemMenu::updateDiscriptionMessage()
{
	const rpg2k::model::DataBase& ldb = field_.project().getLDB();
	descriptionWindow_.clearMessages();
	itemNameWindow_.clearMessages();
	itemNumWindow_.clearMessages();
	switch (state_) {
	case kStateItem:
		if (!itemList_.empty() && itemList_[itemMenu_.cursor()] > 0)
			descriptionWindow_.addLine(ldb.item()[itemList_[itemMenu_.cursor()]][2].to_string().toSystem());
		break;
	case kStateChara:
		if (!itemList_.empty() && itemList_[itemMenu_.cursor()] > 0) {
			itemNameWindow_.addLine(ldb.item()[itemList_[itemMenu_.cursor()]][1].to_string().toSystem());
			char temp[256];
			sprintf(temp, "%s %d", ldb[10].to_string().toSystem().c_str(),
				field_.project().getLSD().itemNum(itemList_[itemMenu_.cursor()]));
			itemNumWindow_.addLine(temp);
		}
		break;
	}
}

void GameItemMenu::updateItemWindow()
{
	const rpg2k::model::DataBase& ldb = field_.project().getLDB();
	rpg2k::model::SaveData& lsd = field_.project().getLSD();
	itemList_.clear();
	itemMenu_.clearMessages();
	char temp[256];
	for(rpg2k::structure::Array2D::ConstIterator it = ldb.item().begin(); it != ldb.item().end(); ++it) {
		if (lsd.itemNum(it->first) > 0) {
			itemList_.push_back(it->first);
			sprintf(temp, "%s : %2d", (*it->second)[1].to_string().toSystem().c_str(), lsd.itemNum(it->first));
			itemMenu_.addLine(temp);
		}
	}
}

void GameItemMenu::start()
{
	freeze(false);
	setState(kStateItem);
}

void GameItemMenu::render(kuto::Graphics2D& g) const
{
}
