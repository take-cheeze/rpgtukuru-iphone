/**
 * @file
 * @brief Save Menu
 * @author project.kuto
 */

#include <kuto/kuto_file.h>
#include <kuto/kuto_stringstream.h>
#include "game_saveload_menu.h"
#include "game_system.h"
#include "game_select_window.h"
#include "game_message_window.h"

using namespace rpg2kLib::model;


GameSaveLoadMenu::GameSaveLoadMenu()
: gameSystem_(NULL), state_(kStateInit), topMenu_(NULL), descriptionWindow_(NULL)
, modeSave_(false)
{
}

void GameSaveLoadMenu::create(kuto::Task* parent, GameSystem& gameSystem, bool modeSave)
{
	gameSystem_ = &gameSystem;
	modeSave_ = modeSave;
	
	topMenu_ = GameSelectWindow::createTask(parent, *gameSystem_);
	topMenu_->pauseUpdate(true);
	topMenu_->setPosition(kuto::Vector2(0.f, 32.f));
	topMenu_->setSize(kuto::Vector2(320.f, 208.f));
	topMenu_->setRowHeight(64.f);
	topMenu_->setLineSpace(0.f);
	topMenu_->setAutoClose(false);
	readHeaders();

	const DataBase& ldb = gameSystem_->getRpgLdb();
	std::ostringstream ss;
	initStringStream(ss);
	for (u32 i = 0; i < SAVE_MAX; i++) {
		ss.str("");
		ss << "Save" << std::setw(2) << (i+1) << " ";
		if (enableHeaders_[i])
			ss << headers_[i].leaderName_ << " " << ldb.getVocabulary()[0x80].get_string() << " " << headers_[i].leaderLevel_;
		else
			ss << "Empty";
		topMenu_->addMessage(ss.str(), modeSave_ || enableHeaders_[i]);
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
	const DataBase& ldb = gameSystem_->getRpgLdb();
	descriptionWindow_->clearMessages();
	descriptionWindow_->addMessage(ldb.getVocabulary()[modeSave_ ? 0x92 : 0x93]);
}

void GameSaveLoadMenu::start()
{
	topMenu_->reset();
	state_ = kStateTop;
}

void GameSaveLoadMenu::readHeaders()
{
	// const DataBase& ldb = gameSystem_->getRpgLdb();
	std::ostringstream ss;
	initStringStream(ss);
	ss.str("");
	ss << kuto::Directory::getHomeDirectory() << "/Documents/"
		<< kuto::File::getFileName(gameSystem_->getRpgLdb().getRootFolder());
	std::string dir = ss.str();
	std::memset(enableHeaders_, 0, sizeof(enableHeaders_));
	if ( kuto::Directory::exists( dir.c_str() ) ) {
		for (u32 i = 0; i < SAVE_MAX; i++) {
			ss.str("");
			ss << dir << "Save" << std::setw(2) << (i+1) << ".lsdi";
			FILE* fp = fopen(ss.str().c_str(), "rb");
			if (fp) {
				size_t siz = fread(&headers_[i], sizeof(GameSaveDataHeader), 1, fp);
				siz = siz;
				fclose(fp);
				enableHeaders_[i] = true;
			}
		}

	}
}

