/**
 * @file
 * @brief Save Menu
 * @author project.kuto
 */

#include <kuto/kuto_render_manager.h>
#include <kuto/kuto_graphics2d.h>
#include <kuto/kuto_file.h>

#include "game.h"
#include "game_chara_status.h"
#include "game_equip_menu.h"
#include "game_field.h"
#include "game_message_window.h"
#include "game_select_window.h"


GameEquipMenu::GameEquipMenu(GameField& gameField, int const charaID)
: GameSystemMenuBase(gameField)
, equipMenu_( *addChild(GameSelectWindow::createTask(field_.game())) )
, itemMenu_( *addChild(GameSelectWindow::createTask(field_.game())) )
, descriptionWindow_( *addChild(GameMessageWindow::createTask(field_.game())) )
, statusWindow_( *addChild(GameMessageWindow::createTask(field_.game())) )
, charaID_(charaID)
{
	equipMenu_.pauseUpdate();
	equipMenu_.setPosition(kuto::Vector2(124.f, 32.f));
	equipMenu_.setSize(kuto::Vector2(196.f, 96.f));
	equipMenu_.setAutoClose(false);

	itemMenu_.pauseUpdate();
	itemMenu_.setPosition(kuto::Vector2(0.f, 128.f));
	itemMenu_.setSize(kuto::Vector2(320.f, 112.f));
	itemMenu_.setAutoClose(false);
	itemMenu_.setColumnSize(2);
	itemMenu_.setPauseUpdateCursor(true);
	itemMenu_.setShowCursor(false);

	statusWindow_.pauseUpdate();
	statusWindow_.setPosition(kuto::Vector2(0.f, 32.f));
	statusWindow_.setSize(kuto::Vector2(124.f, 96.f));
	statusWindow_.enableClick(false);
	statusWindow_.useAnimation(false);
	statusWindow_.addLine(field_.project().character(charaID_).name());

	descriptionWindow_.pauseUpdate();
	descriptionWindow_.setPosition(kuto::Vector2(0.f, 0.f));
	descriptionWindow_.setSize(kuto::Vector2(320.f, 32.f));
	descriptionWindow_.enableClick(false);
	descriptionWindow_.useAnimation(false);
	setDiscriptionMessage();
}

bool GameEquipMenu::initialize()
{
	if (isInitializedChildren()) {
		equipMenu_.pauseUpdate(false);
		itemMenu_.pauseUpdate(false);
		descriptionWindow_.pauseUpdate(false);
		statusWindow_.pauseUpdate(false);
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
		if (equipMenu_.selected()) {
			setState(kStateItem);
		} else if (equipMenu_.canceled()) {
			setState(kStateNone);
		}
		break;
	case kStateItem:
		setDiscriptionMessage();
		if (itemMenu_.selected()) {
			uint16_t oldId = itemList_[itemMenu_.cursor()];
			std::swap( field_.project().character(charaID_).equip()[equipMenu_.cursor()], oldId );
			if (oldId > 0)
				field_.project().getLSD().addItemNum(oldId, 1);
			if (itemList_[itemMenu_.cursor()] > 0)
				field_.project().getLSD().addItemNum(itemList_[itemMenu_.cursor()], -1);
			setState(kStateEquip);
		} else if (itemMenu_.canceled()) {
			setState(kStateEquip);
		}
		break;
	}
}

void GameEquipMenu::setState(int newState)
{
	state_ = newState;
	const rpg2k::model::DataBase& ldb = field_.project().getLDB();
	switch (state_) {
	case kStateEquip:
		{
			itemMenu_.setPauseUpdateCursor(true);
			itemMenu_.setShowCursor(false);
			equipMenu_.clearMessages();
			rpg2k::model::Project::Character::Equip const& equip =
				field_.project().character(charaID_).equip();
			const rpg2k::structure::Array1D& player = ldb.character()[charaID_];
			for(int i = rpg2k::Equip::BEGIN; i < rpg2k::Equip::END; i++) {
				if( (i == rpg2k::Equip::SHIELD) && player[21].to<bool>() ) equipMenu_.addLine(
					ldb.vocabulary(136).toSystem() +
					(equip[i] ? ( " " + ldb.item()[ equip[i] ][1].to_string().toSystem() ) : std::string())
				);
				else equipMenu_.addLine(
					ldb.vocabulary(136 + i) +
					(equip[i] ? ( " " + ldb.item()[ equip[i] ][1].to_string().toSystem() ) : std::string())
				);
			}
			equipMenu_.setPauseUpdateCursor(false);
			equipMenu_.reset();
		}
		break;
	case kStateItem:
		itemMenu_.setPauseUpdateCursor(false);
		equipMenu_.setPauseUpdateCursor(true);
		itemMenu_.setShowCursor(true);
		itemMenu_.reset();
		break;
	}
}

void GameEquipMenu::setDiscriptionMessage()
{
	const rpg2k::model::DataBase& ldb = field_.project().getLDB();
	descriptionWindow_.clearMessages();
	int itemID = 0;
	switch (state_) {
	case kStateEquip:
		itemID = field_.project().character(charaID_).equip()[equipMenu_.cursor()];
		break;
	case kStateItem:
		itemID = itemList_.empty() ? 0 : itemList_[itemMenu_.cursor()];
		break;
	}
	if( itemID ) descriptionWindow_.addLine(ldb.item()[itemID][2].to_string().toSystem());
}

void GameEquipMenu::updateItemWindow()
{
	const rpg2k::model::DataBase& ldb = field_.project().getLDB();
	rpg2k::model::Project& sys = field_.project();
	itemList_.clear();
	itemMenu_.clearMessages();
	char temp[256];
	for(rpg2k::structure::Array2D::ConstIterator it = ldb.item().begin(); it != ldb.item().end(); ++it) {
		if (sys.getLSD().itemNum(it->first) > 0) {
			itemList_.push_back(it->first);
			sprintf(temp, " :%2d", sys.getLSD().itemNum(it->first));
			itemMenu_.addLine((*it->second)[1].to_string().toSystem() + temp);
		}
	}
	itemList_.push_back(0);		// push empty. it is a symbol of unequip.
	itemMenu_.addLine("");
}

void GameEquipMenu::start()
{
	freeze(false);
	setState(kStateEquip);
}

void GameEquipMenu::render(kuto::Graphics2D& g) const
{
}
