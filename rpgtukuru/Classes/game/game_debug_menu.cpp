/**
 * @file
 * @brief Save Menu
 * @author project.kuto
 */

#include <kuto/kuto_virtual_pad.h>
#include "game_debug_menu.h"
#include "game_system.h"
#include "game_field.h"
#include "game_select_window.h"
#include "game_message_window.h"
#include "game_chara_status.h"
#include "game_inventory.h"
#include "game_chara_select_menu.h"
#include "game_player.h"

namespace {

struct DebugTypeInfo {
	const char*		name;
	const char*		description;
	bool			useCharaMenu;
};

const DebugTypeInfo sDebugInfo[] =
{
	{ "Cure Party",	"回復します",					true, },	//	kDebugCure,
	{ "Kill Party",	"殺します",					true, },	//	kDebugKill,
	{ "Level +1",		"1レベルアップします",			true, },	//	kDebugLevelUp,
	{ "Level -1",		"1レベルダウンします",			true, },	//	kDebugLevelDown,
	{ "Money +1000",	"お金を1000足します",			false, },	//	kDebugMoneyUp,
	{ "Money -1000",	"お金を1000引きます",			false, },	//	kDebugMoneyDown,
	{ "Item Up",		"全アイテムを1つ追加します",	false, },	//	kDebugItemUp,
	{ "Item Down",	"全アイテムを1つ減らします",	false, },	//	kDebugItemDown,
	{ "No Encount",			"エンカウントしません",		false, },	//	kDebugNoEncount,
	{ "Always Escape",		"可能な限り逃げられます",	false, },	//	kDebugAlwaysEscape,
	{ "Player Dash",			"ダッシュします",			false, },	//	kDebugPlayerDash,
	{ "Through Collision",	"コリジョン判定しません",	false, },	//	kDebugThroughCollision,
	{ "Dificulty",			"難易度を調節します",		false, },	//	kDebugThroughCollision,
};

}	// namespace


GameDebugMenu::GameDebugMenu(GameField* gameField)
: kuto::Task(gameField->getParent()), gameField_(gameField)
{
	topMenu_ = GameSelectWindow::createTask(this, gameField_->getGameSystem());
	topMenu_->pauseUpdate(true);
	topMenu_->setPosition(kuto::Vector2(0.f, 32.f));
	topMenu_->setSize(kuto::Vector2(320.f, 208.f));
	topMenu_->setAutoClose(false);
	topMenu_->setColumnSize(2);

	descriptionWindow_ = GameMessageWindow::createTask(this, gameField_->getGameSystem());
	descriptionWindow_->pauseUpdate(true);
	descriptionWindow_->setPosition(kuto::Vector2(0.f, 0.f));
	descriptionWindow_->setSize(kuto::Vector2(320.f, 32.f));
	descriptionWindow_->setEnableClick(false);
	descriptionWindow_->setUseAnimation(false);

	charaMenu_ = GameCharaSelectMenu::createTask(this, gameField_);
	charaMenu_->pauseUpdate(true);
	charaMenu_->setPosition(kuto::Vector2(136.f, 0.f));
	charaMenu_->setSize(kuto::Vector2(184.f, 240.f));
	charaMenu_->setAutoClose(false);
	charaMenu_->setUseFullSelectKey(true);

	debugNameWindow_ = GameMessageWindow::createTask(this, gameField_->getGameSystem());
	debugNameWindow_->pauseUpdate(true);
	debugNameWindow_->setPosition(kuto::Vector2(0.f, 0.f));
	debugNameWindow_->setSize(kuto::Vector2(136.f, 32.f));
	debugNameWindow_->setEnableClick(false);
	debugNameWindow_->setUseAnimation(false);
}

void GameDebugMenu::updateTopMenu()
{
	topMenu_->clearMessages();
/*
	for (int i = 0; i < kDebugMax; i++) {
		std::string prefix, postfix;
		if ((i == kDebugNoEncount && gameField_->getGameSystem().getConfig().noEncount) ||
		(i == kDebugAlwaysEscape && gameField_->getGameSystem().getConfig().alwaysEscape) ||
		(i == kDebugPlayerDash && gameField_->getGameSystem().getConfig().playerDash) ||
		(i == kDebugThroughCollision && gameField_->getGameSystem().getConfig().throughCollision))
			prefix = "*";
		if (i == kDebugDifficulty) {
			switch (gameField_->getGameSystem().getConfig().difficulty) {
			case GameConfig::kDifficultyEasy:	postfix = "  Easy";		break;
			case GameConfig::kDifficultyNormal:	postfix = "  Normal";	break;
			case GameConfig::kDifficultyHard:	postfix = "  Hard";		break;
			default:  break;
			}
		}
		topMenu_->addLine(prefix + sDebugInfo[i].name + postfix);
	}
 */
}

bool GameDebugMenu::initialize()
{
	if (isInitializedChildren()) {
		topMenu_->pauseUpdate(false);
		descriptionWindow_->pauseUpdate(false);
		charaMenu_->pauseUpdate(false);
		charaMenu_->freeze(true);
		debugNameWindow_->pauseUpdate(false);
		debugNameWindow_->freeze(true);
		setState(kStateNone);
		return true;
	}
	return false;
}

void GameDebugMenu::update()
{
	kuto::VirtualPad* virtualPad = kuto::VirtualPad::instance();
	switch (state_) {
	case kStateNone:
		if (virtualPad->repeat(kuto::VirtualPad::KEY_X)) {
			setState(kStateTop);
		}
		break;
	case kStateTop:
		updateDiscriptionMessage();
		if (virtualPad->repeat(kuto::VirtualPad::KEY_X)) {
			setState(kStateNone);
		} else if (topMenu_->selected()) {
			if (sDebugInfo[topMenu_->cursor()].useCharaMenu) {
				setState(kStateChara);
			} else {
				applyDebug(topMenu_->cursor(), 0);
				topMenu_->reset();
			}
		} else if (topMenu_->canceled()) {
			setState(kStateNone);
		}
		break;
	case kStateChara:
		updateDiscriptionMessage();
		if (virtualPad->repeat(kuto::VirtualPad::KEY_X)) {
			setState(kStateNone);
		} else if (charaMenu_->selected()) {
			int playerId = charaMenu_->isFullSelect()? 0 : gameField_->getPlayers()[charaMenu_->cursor()]->getPlayerId();
			applyDebug(topMenu_->cursor(), playerId);
			charaMenu_->reset();
		} else if (charaMenu_->canceled()) {
			setState(kStateTop);
		}
		break;
	}
}

void GameDebugMenu::applyDebug(int debugId, int playerId)
{
	rpg2k::model::Project& system = gameField_->getGameSystem();
	kuto::StaticVector<GamePlayer*, 4> playerList;
	if (playerId == 0) {
		// party all
		for (uint i = 0; i < gameField_->getPlayers().size(); i++)
			playerList.push_back(gameField_->getPlayers()[i]);
	} else {
		playerList.push_back(gameField_->getPlayerFromId(playerId));
	}
	switch (debugId) {
	case kDebugCure:
		for (uint i = 0; i < playerList.size(); i++) {
			playerList[i]->getStatus().fullCure();
		}
		break;
	case kDebugKill:
		for (uint i = 0; i < playerList.size(); i++) {
			playerList[i]->getStatus().kill();
		}
		break;
	case kDebugLevelUp:
		for (uint i = 0; i < playerList.size(); i++) {
			playerList[i]->getStatus().addLevel(1);
		}
		break;
	case kDebugLevelDown:
		for (uint i = 0; i < playerList.size(); i++) {
			playerList[i]->getStatus().addLevel(-1);
		}
		break;
	case kDebugMoneyUp:
		system.getLSD().addMoney(1000);
		break;
	case kDebugMoneyDown:
		system.getLSD().addMoney(-1000);
		break;
	case kDebugItemUp:
		for (uint i = 1; i <= system.getLDB().item().rbegin().first(); i++) {
			system.getLSD().addItemNum(i, 1);
		}
		break;
	case kDebugItemDown:
		for (uint i = 1; i <= system.getLDB().item().rbegin().first(); i++) {
			system.getLSD().addItemNum(i, -1);
		}
		break;
/*
	case kDebugNoEncount:
		system.getConfig().noEncount = !system.getConfig().noEncount;
		break;
	case kDebugAlwaysEscape:
		system.getConfig().alwaysEscape = !system.getConfig().alwaysEscape;
		break;
	case kDebugPlayerDash:
		system.getConfig().playerDash = !system.getConfig().playerDash;
		break;
	case kDebugThroughCollision:
		system.getConfig().throughCollision = !system.getConfig().throughCollision;
		break;
	case kDebugDifficulty:
		system.getConfig().difficulty = (GameConfig::Difficulty)((system.getConfig().difficulty + 1) % GameConfig::kDifficultyMax);
		break;
 */
	default: rpg2k_assert(false);
	}
	updateTopMenu();
}

void GameDebugMenu::setState(State newState)
{
	state_ = newState;
	switch (state_) {
	case kStateNone:
		gameField_->freeze(false);
		topMenu_->freeze(true);
		descriptionWindow_->freeze(true);
		charaMenu_->freeze(true);
		debugNameWindow_->freeze(true);
		break;
	case kStateTop:
		updateTopMenu();
		gameField_->freeze(true);
		topMenu_->freeze(false);
		descriptionWindow_->freeze(false);
		charaMenu_->freeze(true);
		debugNameWindow_->freeze(true);
		topMenu_->setPauseUpdateCursor(false);
		topMenu_->setShowCursor(true);
		topMenu_->reset();
		break;
	case kStateChara:
		topMenu_->freeze(true);
		descriptionWindow_->freeze(true);
		charaMenu_->freeze(false);
		debugNameWindow_->freeze(false);
		charaMenu_->reset();
		break;
	}
}

void GameDebugMenu::updateDiscriptionMessage()
{
	descriptionWindow_->clearMessages();
	debugNameWindow_->clearMessages();
	switch (state_) {
	case kStateTop:
		descriptionWindow_->addLine(sDebugInfo[topMenu_->cursor()].description);
		break;
	case kStateChara:
		debugNameWindow_->addLine(sDebugInfo[topMenu_->cursor()].name);
		break;
	default: break;
	}
}

