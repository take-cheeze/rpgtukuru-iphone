/**
 * @file
 * @brief Save Menu
 * @author project.kuto
 */

#include <kuto/kuto_virtual_pad.h>
#include <rpg2k/Project.hpp>

#include "game.h"
#include "game_debug_menu.h"
#include "game_field.h"
#include "game_select_window.h"
#include "game_message_window.h"
#include "game_chara_status.h"
#include "game_chara_select_menu.h"

#include <algorithm>
#include <iterator>

namespace {

struct DebugTypeInfo {
	const char*		name;
	const char*		description;
	bool			useCharaMenu;
} const sDebugInfo[] = {
	{ "Cure Party",			"回復します",				true , },	//	kDebugCure,
	{ "Kill Party",			"殺します",					true , },	//	kDebugKill,
	{ "Level +1",			"1レベルアップします",		true , },	//	kDebugLevelUp,
	{ "Level -1",			"1レベルダウンします",		true , },	//	kDebugLevelDown,
	{ "Money +1000",		"お金を1000足します",			false, },	//	kDebugMoneyUp,
	{ "Money -1000",		"お金を1000引きます",			false, },	//	kDebugMoneyDown,
	{ "Item Up",			"全アイテムを1つ追加します",	false, },	//	kDebugItemUp,
	{ "Item Down",			"全アイテムを1つ減らします",	false, },	//	kDebugItemDown,
	{ "No Encount",			"エンカウントしません",		false, },	//	kDebugNoEncount,
	{ "Always Escape",		"可能な限り逃げられます",		false, },	//	kDebugAlwaysEscape,
	{ "Player Dash",		"ダッシュします",				false, },	//	kDebugPlayerDash,
	{ "Through Collision",	"コリジョン判定しません",		false, },	//	kDebugThroughCollision,
	{ "Dificulty",			"難易度を調節します",			false, },	//	kDebugThroughCollision,
};

}	// namespace


GameDebugMenu::GameDebugMenu(GameField& gameField)
: kuto::Task(), field_(gameField)
{
	topMenu_ = addChild(GameSelectWindow::createTask(field_.game()));
	topMenu_->pauseUpdate();
	topMenu_->setPosition(kuto::Vector2(0.f, 32.f));
	topMenu_->setSize(kuto::Vector2(320.f, 208.f));
	topMenu_->setAutoClose(false);
	topMenu_->setColumnSize(2);

	descriptionWindow_ = addChild(GameMessageWindow::createTask(field_.game()));
	descriptionWindow_->pauseUpdate();
	descriptionWindow_->setPosition(kuto::Vector2(0.f, 0.f));
	descriptionWindow_->setSize(kuto::Vector2(320.f, 32.f));
	descriptionWindow_->enableClick(false);
	descriptionWindow_->useAnimation(false);

	charaMenu_ = addChild(kuto::TaskCreatorParam1<GameCharaSelectMenu, GameField&>::createTask(field_) );
	charaMenu_->pauseUpdate();
	charaMenu_->setPosition(kuto::Vector2(136.f, 0.f));
	charaMenu_->setSize(kuto::Vector2(184.f, 240.f));
	charaMenu_->setAutoClose(false);
	charaMenu_->useFullSelectKey(true);

	debugNameWindow_ = addChild(GameMessageWindow::createTask(field_.game()));
	debugNameWindow_->pauseUpdate();
	debugNameWindow_->setPosition(kuto::Vector2(0.f, 0.f));
	debugNameWindow_->setSize(kuto::Vector2(136.f, 32.f));
	debugNameWindow_->enableClick(false);
	debugNameWindow_->useAnimation(false);
}

void GameDebugMenu::updateTopMenu()
{
	topMenu_->clearMessages();
	GameConfig const& config = field_.game().config();
	for (int i = 0; i < kDebugMax; i++) {
		std::string prefix, postfix;
		if ((i == kDebugNoEncount && config.noEncount) ||
		(i == kDebugAlwaysEscape && config.alwaysEscape) ||
		(i == kDebugPlayerDash && config.playerDash) ||
		(i == kDebugThroughCollision && config.throughCollision))
			prefix = "*";
		if (i == kDebugDifficulty) {
			switch (config.difficulty) {
			case GameConfig::kDifficultyEasy:	postfix = "  Easy";		break;
			case GameConfig::kDifficultyNormal:	postfix = "  Normal";	break;
			case GameConfig::kDifficultyHard:	postfix = "  Hard";		break;
			default:  break;
			}
		}
		topMenu_->addLine(prefix + sDebugInfo[i].name + postfix);
	}
}

bool GameDebugMenu::initialize()
{
	if (isInitializedChildren()) {
		topMenu_->pauseUpdate(false);
		descriptionWindow_->pauseUpdate(false);
		charaMenu_->pauseUpdate(false);
		charaMenu_->freeze();
		debugNameWindow_->pauseUpdate(false);
		debugNameWindow_->freeze();
		setState(kStateNone);
		return true;
	}
	return false;
}

void GameDebugMenu::update()
{
	kuto::VirtualPad& virtualPad = kuto::VirtualPad::instance();
	switch (state_) {
	case kStateNone:
		if (virtualPad.repeat(kuto::VirtualPad::KEY_X)) {
			setState(kStateTop);
		}
		break;
	case kStateTop:
		updateDiscriptionMessage();
		if (virtualPad.repeat(kuto::VirtualPad::KEY_X)) {
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
		if (virtualPad.repeat(kuto::VirtualPad::KEY_X)) {
			setState(kStateNone);
		} else if (charaMenu_->selected()) {
			int playerId = charaMenu_->isFullSelect()? 0 : field_.project().getLSD().member()[charaMenu_->cursor()];
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
	rpg2k::model::Project& proj = field_.project();
	GameConfig& config = field_.game().config();
	kuto::StaticVector<unsigned, rpg2k::MEMBER_MAX> playerList;
	if (playerId == 0) {
		// party all
		std::vector<uint16_t> const& mem = proj.getLSD().member();
		std::copy( mem.begin(), mem.end(), std::back_inserter(playerList) );
	} else { playerList.push_back(playerId); }
	switch (debugId) {
	case kDebugCure:
		for (uint i = 0; i < playerList.size(); i++) {
			proj.character(playerList[i]).cure();
		}
		break;
	case kDebugKill:
		for (uint i = 0; i < playerList.size(); i++) {
			proj.character(playerList[i]).setHP(0);
		}
		break;
	case kDebugLevelUp:
		for (uint i = 0; i < playerList.size(); i++) {
			proj.character(playerList[i]).addLevel(1);
		}
		break;
	case kDebugLevelDown:
		for (uint i = 0; i < playerList.size(); i++) {
			proj.character(playerList[i]).addLevel(-1);
		}
		break;
	case kDebugMoneyUp:
		proj.getLSD().addMoney(1000);
		break;
	case kDebugMoneyDown:
		proj.getLSD().addMoney(-1000);
		break;
	case kDebugItemUp:
	case kDebugItemDown:
		for (rpg2k::structure::Array2D::Iterator it = proj.getLDB().item().begin(); it != proj.getLDB().item().end(); ++it) {
			proj.getLSD().addItemNum(it->first, (debugId == kDebugItemUp)? 1 : -1);
		}
		break;
	case kDebugNoEncount:
		config.noEncount = !config.noEncount;
		break;
	case kDebugAlwaysEscape:
		config.alwaysEscape = !config.alwaysEscape;
		break;
	case kDebugPlayerDash:
		config.playerDash = !config.playerDash;
		break;
	case kDebugThroughCollision:
		config.throughCollision = !config.throughCollision;
		break;
	case kDebugDifficulty:
		config.difficulty = (GameConfig::Difficulty)((config.difficulty + 1) % GameConfig::kDifficultyMax);
		break;
	default: rpg2k_assert(false);
	}
	updateTopMenu();
}

void GameDebugMenu::setState(State newState)
{
	state_ = newState;
	switch (state_) {
	case kStateNone:
		field_.freeze(false);
		topMenu_->freeze();
		descriptionWindow_->freeze();
		charaMenu_->freeze();
		debugNameWindow_->freeze();
		break;
	case kStateTop:
		updateTopMenu();
		field_.freeze();
		topMenu_->freeze(false);
		descriptionWindow_->freeze(false);
		charaMenu_->freeze();
		debugNameWindow_->freeze();
		topMenu_->setPauseUpdateCursor(false);
		topMenu_->setShowCursor(true);
		topMenu_->reset();
		break;
	case kStateChara:
		topMenu_->freeze();
		descriptionWindow_->freeze();
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

