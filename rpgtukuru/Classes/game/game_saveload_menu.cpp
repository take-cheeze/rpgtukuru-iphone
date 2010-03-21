/**
 * @file
 * @brief Save Menu
 * @author project.kuto
 */

#include <kuto/kuto_file.h>
#include "game_saveload_menu.h"
#include "game_system.h"
#include "game_select_window.h"
#include "game_message_window.h"


GameSaveLoadMenu::GameSaveLoadMenu()
: gameSystem_(NULL), state_(kStateInit)
, topMenu_(NULL), descriptionWindow_(NULL)
, modeSave_(false)
{
}

void GameSaveLoadMenu::create(kuto::Task* parent, GameSystem& gameSystem, bool modeSave)
{
	gameSystem_ = &gameSystem;
	modeSave_ = modeSave;
	
	const CRpgLdb& ldb = gameSystem_->getRpgLdb();
	topMenu_ = GameSelectWindow::createTask(parent, *gameSystem_);
	topMenu_->pauseUpdate(true);
	topMenu_->setPosition(kuto::Vector2(0.f, 32.f));
	topMenu_->setSize(kuto::Vector2(320.f, 208.f));
	topMenu_->setRowHeight(64.f);
	topMenu_->setLineSpace(0.f);
	topMenu_->setAutoClose(false);
	readHeaders();
	for (u32 i = 0; i < SAVE_MAX; i++) {
		char temp[256];
		if (enableHeaders_[i])
			sprintf(temp, "Save%02d %s %s %d", i + 1, headers_[i].leaderName_, ldb.term.param.levelShort.c_str(), headers_[i].leaderLevel_);
		else
			sprintf(temp, "Save%02d Empty", i + 1);
		topMenu_->addMessage(temp, modeSave_ || enableHeaders_[i]);
	}

	descriptionWindow_ = GameMessageWindow::createTask(parent, *gameSystem_);
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
	const CRpgLdb& ldb = gameSystem_->getRpgLdb();
	descriptionWindow_->clearMessages();
	descriptionWindow_->addMessage(modeSave_? ldb.term.saveLoad.selectSave : ldb.term.saveLoad.selectLoad);
}

void GameSaveLoadMenu::start()
{
	topMenu_->reset();
	state_ = kStateTop;
}

void GameSaveLoadMenu::readHeaders()
{
	char dirName[256];
	sprintf(dirName, "%s/Documents/%s", kuto::Directory::getHomeDirectory().c_str(),
		kuto::File::getFileName(gameSystem_->getRpgLdb().getRootFolder()).c_str());
	std::memset(enableHeaders_, 0, sizeof(enableHeaders_));
	if (kuto::Directory::exists(dirName)) {
		for (u32 i = 0; i < SAVE_MAX; i++) {
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

