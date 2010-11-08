/**
 * @file
 * @brief GameNameInputMenu
 * @author project.kuto
 */

#include "game.h"
#include "game_field.h"
#include "game_message_window.h"
#include "game_name_input_menu.h"
#include "game_name_select_window.h"


GameNameInputMenu::GameNameInputMenu(Game& g)
: kuto::Task()
, game_(g)
, project_(g.project()), playerId_(0), inputIndex_(0), closed_(false)
{
	selectWindow_ = addChild(kuto::TaskCreatorParam1<GameNameSelectWindow, Game&>::createTask(game_));
	selectWindow_->pauseUpdate();

	charaWindow_ = addChild(GameMessageWindow::createTask(game_));
	charaWindow_->pauseUpdate();
	charaWindow_->setPosition(kuto::Vector2(0.f, 0.f));
	charaWindow_->setSize(kuto::Vector2(80.f, 80.f));
	charaWindow_->enableClick(false);
	charaWindow_->useAnimation(false);
	//const rpg2k::model::DataBase& ldb = field_.project().getLDB();
	//const rpg2k::structure::Array1D& player = ldb.character()[playerId_];
	//charaWindow_->setFaceTexture(player.faceGraphicName, player.faceGraphicPos, false, false);
	//charaWindow_->addLine("");

	nameWindow_ = addChild(GameMessageWindow::createTask(game_));
	nameWindow_->pauseUpdate();
	nameWindow_->setPosition(kuto::Vector2(80.f, 48.f));
	nameWindow_->setSize(kuto::Vector2(240.f, 32.f));
	nameWindow_->enableClick(false);
	nameWindow_->useAnimation(false);
	nameWindow_->addLine("");
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
	// const rpg2k::model::DataBase& ldb = project_.getLDB();
	// const GamePlayerInfo& player = project_.playerInfo(playerId_);
	rpg2k::model::Project::Character const& c = project_.character(playerId_);
	charaWindow_->setFaceTexture(c.faceSet(), c.faceSetPos(), false, false);

	selectWindow_->setKana(katakana? GameNameSelectWindow::kKatakana : GameNameSelectWindow::kHiragana);

	nameWindow_->clearMessages();
	nameWindow_->addLine(useDefaultName? c.name() : "");

	selectWindow_->reset();
	selectWindow_->resetCursor();
	closed_ = false;
}

void GameNameInputMenu::update()
{
	if (selectWindow_->selected()) {
		if (selectWindow_->cursor() == GameNameSelectWindow::kApplyButton) {
			// ok
			project_.getLSD().character()[playerId_][1] = nameWindow_->message(0).str;
			name_ = nameWindow_->message(0).str;
			closed_ = true;
		} else {
			if (nameWindow_->messageLength() < 6) {
				std::string name = nameWindow_->message(0).str;
				nameWindow_->clearMessages();
				name += selectWindow_->message(selectWindow_->cursor()).str;
				nameWindow_->addLine(name);
			}
			selectWindow_->reset();
		}
	} else if (selectWindow_->canceled()) {
		std::string name = nameWindow_->message(0).str;
		if (!name.empty()) {
			int len = nameWindow_->messageLength();
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
			nameWindow_->addLine(name);
			selectWindow_->reset();
		}
	}
}
