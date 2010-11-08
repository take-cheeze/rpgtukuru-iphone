/**
 * @file
 * @brief Save Menu
 * @author project.kuto
 */

#include <kuto/kuto_file.h>

#include <iomanip>
#include <sstream>

#include <rpg2k/Project.hpp>

#include "game.h"
#include "game_message_window.h"
#include "game_saveload_menu.h"
#include "game_select_window.h"


GameSaveLoadMenu::GameSaveLoadMenu(Game& g, bool modeSave)
: game_(g), project_(g.project()), state_(kStateInit)
, topMenu_( *addChild(GameSelectWindow::createTask(g)) )
, descriptionWindow_( *addChild(GameMessageWindow::createTask(g)) )
, modeSave_(modeSave)
{
	const rpg2k::model::DataBase& ldb = project_.getLDB();
	topMenu_.pauseUpdate();
	topMenu_.setPosition(kuto::Vector2(0.f, 32.f));
	topMenu_.setSize(kuto::Vector2(320.f, 208.f));
	topMenu_.setRowHeight(64.f);
	topMenu_.setLineSpace(0.f);
	topMenu_.setAutoClose(false);
	// readHeaders();
	for (uint i = 0; i < rpg2k::SAVE_DATA_MAX; i++) {
		std::ostringstream oss;
		rpg2k::model::SaveData& lsd = project_.getLSD(i + 1);
		oss << "Save" << std::setw(2) << (i + 1) << " ";
		if ( lsd.exists() ) {
			rpg2k::structure::Array1D& preview = lsd[100];
			oss << preview[11].to_string().toSystem() << " " << ldb.vocabulary(128).toSystem() << std::setw(2) << preview[12].to<int>();
		} else {
			oss << "Empty";
		}
		topMenu_.addLine(oss.str(), modeSave_ || lsd.exists());
	}

	descriptionWindow_.pauseUpdate();
	descriptionWindow_.setPosition(kuto::Vector2(0.f, 0.f));
	descriptionWindow_.setSize(kuto::Vector2(320.f, 32.f));
	descriptionWindow_.enableClick(false);
	descriptionWindow_.useAnimation(false);
	setDiscriptionMessage();
}

bool GameSaveLoadMenu::initialize()
{
	if (state_ == kStateInit && topMenu_.isInitialized() && descriptionWindow_.isInitialized()) {
		topMenu_.pauseUpdate(false);
		descriptionWindow_.pauseUpdate(false);
		state_ = kStateNone;
		start();
	}
	return state_ != kStateInit;
}

bool GameSaveLoadMenu::selected() const
{
	return topMenu_.selected();
}

bool GameSaveLoadMenu::canceled() const
{
	return topMenu_.canceled();
}

int GameSaveLoadMenu::selectIndex() const
{
	return topMenu_.cursor();
}

void GameSaveLoadMenu::setDiscriptionMessage()
{
	const rpg2k::model::DataBase& ldb = project_.getLDB();
	descriptionWindow_.clearMessages();
	descriptionWindow_.addLine(ldb.vocabulary(modeSave_? 146 : 147).toSystem());
}

void GameSaveLoadMenu::start()
{
	topMenu_.reset();
	state_ = kStateTop;
}

void GameSaveLoadMenu::update()
{
}
