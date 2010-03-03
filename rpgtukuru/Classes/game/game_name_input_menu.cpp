/**
 * @file
 * @brief GameNameInputMenu
 * @author project.kuto
 */

#include "game_name_input_menu.h"
#include "game_system.h"
#include "game_field.h"
#include "game_name_select_window.h"
#include "game_message_window.h"


GameNameInputMenu::GameNameInputMenu(kuto::Task* parent, GameSystem& gameSystem)
: kuto::Task(parent)
, gameSystem_(gameSystem), playerId_(0), inputIndex_(0), closed_(false)
{
	selectWindow_ = GameNameSelectWindow::createTask(this, gameSystem);
	selectWindow_->pauseUpdate(true);

	charaWindow_ = GameMessageWindow::createTask(this, gameSystem);
	charaWindow_->pauseUpdate(true);
	charaWindow_->setPosition(kuto::Vector2(0.f, 0.f));
	charaWindow_->setSize(kuto::Vector2(80.f, 80.f));
	charaWindow_->setEnableClick(false);
	charaWindow_->setUseAnimation(false);
	//const CRpgLdb& ldb = gameField_->getGameSystem().getRpgLdb();
	//const CRpgLdb::Player& player = ldb.saPlayer[playerId_];
	//charaWindow_->setFaceTexture(player.faceGraphicName, player.faceGraphicPos, false, false);
	//charaWindow_->addMessage("");
	
	nameWindow_ = GameMessageWindow::createTask(this, gameSystem);
	nameWindow_->pauseUpdate(true);
	nameWindow_->setPosition(kuto::Vector2(80.f, 48.f));
	nameWindow_->setSize(kuto::Vector2(240.f, 32.f));
	nameWindow_->setEnableClick(false);
	nameWindow_->setUseAnimation(false);
	nameWindow_->addMessage("");
}

GameNameInputMenu::~GameNameInputMenu()
{
}

bool GameNameInputMenu::initialize()
{
	if (isInitializedChildren()) {
		selectWindow_->pauseUpdate(false);
		charaWindow_->pauseUpdate(false);
		nameWindow_->pauseUpdate(false);
		return true;
	}
	return false;
}

void GameNameInputMenu::setPlayerInfo(int playerId, bool katakana, bool useDefaultName)
{
	playerId_ = playerId;
	const CRpgLdb& ldb = gameSystem_.getRpgLdb();
	const GamePlayerInfo& player = gameSystem_.getPlayerInfo(playerId_);
	charaWindow_->setFaceTexture(player.faceGraphicName, player.faceGraphicPos, false, false);
	
	selectWindow_->setKana(katakana? GameNameSelectWindow::kKatakana : GameNameSelectWindow::kHiragana);
	
	nameWindow_->clearMessages();
	nameWindow_->addMessage(useDefaultName? player.name : "");
	
	selectWindow_->reset();
	selectWindow_->resetCursor();
	closed_ = false;
}

void GameNameInputMenu::update()
{
	if (selectWindow_->selected()) {
		if (selectWindow_->cursor() == GameNameSelectWindow::kApplyButton) {
			// ok
			gameSystem_.getPlayerInfo(playerId_).name = nameWindow_->getMessage(0).str;
			name_ = nameWindow_->getMessage(0).str;
			closed_ = true;
		} else {
			if (nameWindow_->getMessageLength() < 6) {
				std::string name = nameWindow_->getMessage(0).str;
				nameWindow_->clearMessages();
				name += selectWindow_->getMessage(selectWindow_->cursor()).str;
				nameWindow_->addMessage(name);
			}
			selectWindow_->reset();
		}
	} else if (selectWindow_->canceled()) {
		std::string name = nameWindow_->getMessage(0).str;
		if (!name.empty()) {
			int len = nameWindow_->getMessageLength();
			int tailIndex = 0;
			while (len > 1) {
				char c = name[tailIndex++];
				if (c & 0x80) {
					if (c & 0x40) {
						tailIndex++;
						if (c & 0x20) {
							tailIndex++;
						}
					}
				}
				len--;
			}
			name = name.substr(0, tailIndex);
			nameWindow_->clearMessages();
			nameWindow_->addMessage(name);
			selectWindow_->reset();
		}
	}
}




