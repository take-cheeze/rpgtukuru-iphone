/**
 * @file
 * @brief Save Menu
 * @author project.kuto
 */

#include <kuto/kuto_file.h>
#include <sstream>

#include "game_saveload_menu.h"
#include "game_system.h"
#include "game_select_window.h"
#include "game_message_window.h"


GameSaveLoadMenu::GameSaveLoadMenu(rpg2k::model::Project& gameSystem, bool modeSave)
: gameSystem_(&gameSystem), state_(kStateInit)
, topMenu_(NULL), descriptionWindow_(NULL)
, modeSave_(modeSave)
{
	const rpg2k::model::DataBase& ldb = gameSystem_->getLDB();
	topMenu_ = addChild(GameSelectWindow::createTask(*gameSystem_));
	topMenu_->pauseUpdate(true);
	topMenu_->setPosition(kuto::Vector2(0.f, 32.f));
	topMenu_->setSize(kuto::Vector2(320.f, 208.f));
	topMenu_->setRowHeight(64.f);
	topMenu_->setLineSpace(0.f);
	topMenu_->setAutoClose(false);
	// readHeaders();
	for (uint i = 0; i < rpg2k::SAVE_DATA_MAX; i++) {
		std::ostringstream oss;
		rpg2k::model::SaveData& lsd = gameSystem.getLSD(i + 1);
		oss << "Save" << std::setw(2) << (i + 1) << " ";
		if ( lsd.exists() ) {
			rpg2k::structure::Array1D& preview = lsd[100];
			oss << preview[11].get_string().toSystem() << " " << ldb.vocabulary(128).toSystem() << std::setw(2) << preview[12].get<int>();
		} else {
			oss << "Empty";
		}
		topMenu_->addLine(oss.str(), modeSave_ || lsd.exists());
	}

	descriptionWindow_ = addChild(GameMessageWindow::createTask(*gameSystem_));
	descriptionWindow_->pauseUpdate(true);
	descriptionWindow_->setPosition(kuto::Vector2(0.f, 0.f));
	descriptionWindow_->setSize(kuto::Vector2(320.f, 32.f));
	descriptionWindow_->setEnableClick(false);
	descriptionWindow_->setUseAnimation(false);
	setDiscriptionMessage();
}

bool GameSaveLoadMenu::initialize()
{
	if (state_ == kStateInit && topMenu_->isInitialized() && descriptionWindow_->isInitialized()) {
		topMenu_->pauseUpdate(false);
		descriptionWindow_->pauseUpdate(false);
		state_ = kStateNone;
		start();
	}
	return state_ != kStateInit;
}

bool GameSaveLoadMenu::selected() const
{
	return topMenu_->selected();
}

bool GameSaveLoadMenu::canceled() const
{
	return topMenu_->canceled();
}

int GameSaveLoadMenu::selectIndex() const
{
	return topMenu_->cursor();
}

void GameSaveLoadMenu::setDiscriptionMessage()
{
	const rpg2k::model::DataBase& ldb = gameSystem_->getLDB();
	descriptionWindow_->clearMessages();
	descriptionWindow_->addLine(modeSave_? ldb.vocabulary(146).toSystem() : ldb.vocabulary(147).toSystem());
}

void GameSaveLoadMenu::start()
{
	topMenu_->reset();
	state_ = kStateTop;
}

void GameSaveLoadMenu::update()
{
}

/*
void GameSaveLoadMenu::readHeaders()
{
	char dirName[256];
	sprintf(dirName, "%s/Documents/%s", kuto::Directory::getHomeDirectory().c_str(),
		kuto::File::getFileName(gameSystem_->getLDB().directory()).c_str());
	std::memset(enableHeaders_, 0, sizeof(enableHeaders_));
	if (kuto::Directory::exists(dirName)) {
		for (uint i = 0; i < SAVE_MAX; i++) {
			char saveName[256];
			sprintf(saveName, "%s/Save%02d.lsdi", dirName, i + 1);
			FILE* fp = fopen(saveName, "r");
			if (fp) {
				size_t readSize = fread(&headers_[i], sizeof(GameSaveDataHeader), 1, fp);
				readSize = readSize;
				fclose(fp);
				enableHeaders_[i] = true;
			}
		}
	}
}
 */
