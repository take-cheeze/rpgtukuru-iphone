/**
 * @file
 * @brief Game Battle Menu
 * @author project.kuto
 */

#include <kuto/kuto_render_manager.h>

#include "game_battle.h"
#include "game_battle_chara.h"
#include "game_battle_menu.h"
#include "game_field.h"
#include "game_message_window.h"
#include "game_select_window.h"

#include <rpg2k/Project.hpp>


GameBattleMenu::GameBattleMenu(GameBattle& battle)
: kuto::IRender2D(kuto::Layer::OBJECT_2D, 0.f)
, battle_(battle), page_(kPageTop)
, descriptionWindow_( *addChild(GameMessageWindow::createTask(battle_.field().game()) ) )
, animationCounter_(0), decided_(false), partyCommand_(kPartyCommandManual)
{
	oldPage_ = page_;
	for (int i = 0; i < kPageMax; i++) {
		selectWindows_[i] = addChild( GameSelectWindow::createTask(battle.field().game()) );
		selectWindows_[i]->pauseUpdate();
		selectWindows_[i]->pauseDraw();
		selectWindows_[i]->setPauseUpdateCursor(true);
		selectWindows_[i]->setPriority(3.f);
		selectWindows_[i]->setAutoClose(false);
	}
	const rpg2k::model::DataBase& ldb = battle_.project().getLDB();
	selectWindows_[kPageTop]->addLine(ldb.vocabulary(101).toSystem());
	selectWindows_[kPageTop]->addLine(ldb.vocabulary(102).toSystem());
	selectWindows_[kPageTop]->addLine(ldb.vocabulary(103).toSystem());
	selectWindows_[kPageTop]->setPosition(kuto::Vector2(0.f, 160.f));
	selectWindows_[kPageTop]->setSize(kuto::Vector2(70.f, 80.f));

	updateCharaWindow();
	selectWindows_[kPageChara]->setPosition(kuto::Vector2(70.f, 160.f));
	selectWindows_[kPageChara]->setSize(kuto::Vector2(250.f, 80.f));

	selectWindows_[kPageCommand]->setPosition(kuto::Vector2(320.f, 160.f));
	selectWindows_[kPageCommand]->setSize(kuto::Vector2(70.f, 80.f));

	selectWindows_[kPageSkill]->setPosition(kuto::Vector2(0.f, 160.f));
	selectWindows_[kPageSkill]->setSize(kuto::Vector2(320.f, 80.f));
	selectWindows_[kPageSkill]->setColumnSize(2);
	selectWindows_[kPageSkill]->setPriority(2.5f);

	selectWindows_[kPageItem]->setPosition(kuto::Vector2(0.f, 160.f));
	selectWindows_[kPageItem]->setSize(kuto::Vector2(320.f, 80.f));
	selectWindows_[kPageItem]->setColumnSize(2);
	selectWindows_[kPageItem]->setPriority(2.5f);

	selectWindows_[kPageTarget]->setPosition(kuto::Vector2(0.f, 160.f));
	selectWindows_[kPageTarget]->setSize(kuto::Vector2(120.f, 80.f));
	selectWindows_[kPageTarget]->setPriority(2.f);

	selectWindows_[kPageTargetFriends]->setPosition(kuto::Vector2(0.f, 160.f));
	selectWindows_[kPageTargetFriends]->setSize(kuto::Vector2(180.f, 80.f));
	selectWindows_[kPageTargetFriends]->setPriority(2.f);

	descriptionWindow_.setPosition(kuto::Vector2(0.f, 0.f));
	descriptionWindow_.setSize(kuto::Vector2(320.f, 32.f));
	descriptionWindow_.enableClick(false);
	descriptionWindow_.useAnimation(false);

	selectWindows_[kPageTop]->pauseUpdate(false);
	selectWindows_[kPageTop]->pauseDraw(false);
	selectWindows_[kPageTop]->setPauseUpdateCursor(false);
	selectWindows_[kPageChara]->pauseUpdate(false);
	selectWindows_[kPageChara]->pauseDraw(false);
	descriptionWindow_.pauseUpdate();
}

void GameBattleMenu::updateCharaWindow()
{
	const rpg2k::model::DataBase& ldb = battle_.project().getLDB();
	selectWindows_[kPageChara]->clearMessages();
	for (uint i = 0; i < battle_.players().size(); i++) {
		char message[256];
		GameBattleChara* chara = battle_.players()[i];
		int badConditionId = chara->worstBadConditionID(false);
		sprintf(message, "%s %s %s%3d/%3d %s%3d/%3d", chara->name().c_str(),
			badConditionId ? ldb.condition()[badConditionId][1].to_string().toSystem().c_str() : ldb.vocabulary(126).toSystem().c_str(),
			ldb.vocabulary(129).toSystem().c_str(), chara->status().hp(), chara->status().baseStatus()[rpg2k::Param::HP],
			ldb.vocabulary(130).toSystem().c_str(), chara->status().mp(), chara->status().baseStatus()[rpg2k::Param::MP]);
		selectWindows_[kPageChara]->addLine(message);
	}
}

bool GameBattleMenu::initialize()
{
	if (isInitializedChildren()) {
		descriptionWindow_.pauseUpdate(false);
		descriptionWindow_.freeze();
		return true;
	}
	return false;
}

void GameBattleMenu::update()
{
	if (animationCounter_ > 0) {
		animationCounter_--;
		switch (page_) {
		case kPageTop:
			selectWindows_[kPageTop]->setPosition(kuto::Vector2(animationCounter_ * 7.f - 70.f, 160.f));
			selectWindows_[kPageChara]->setPosition(kuto::Vector2(animationCounter_ * 7.f, 160.f));
			selectWindows_[kPageCommand]->setPosition(kuto::Vector2(250.f + animationCounter_ * 7.f, 160.f));
			if (animationCounter_ == 0) {
				nextPlayer(0);
			}
			break;
		case kPageCommand:
			selectWindows_[kPageTop]->setPosition(kuto::Vector2(-animationCounter_ * 7.f, 160.f));
			selectWindows_[kPageChara]->setPosition(kuto::Vector2(70.f - animationCounter_ * 7.f, 160.f));
			selectWindows_[kPageCommand]->setPosition(kuto::Vector2(320.f - animationCounter_ * 7.f, 160.f));
			if (animationCounter_ == 0) {
				setPage(kPageTop);
			}
			break;
		}
	} else {
		bool selected = selectWindows_[page_]->selected();
		bool canceled = selectWindows_[page_]->canceled();
		int cursor = selectWindows_[page_]->cursor();

		switch (page_) {
		case kPageTop:
			if (selected) {
				switch (cursor) {
				case 0:		// manual
					partyCommand_ = kPartyCommandManual;
					selectWindows_[kPageCommand]->pauseUpdate(false);
					selectWindows_[kPageCommand]->pauseDraw(false);
					animationCounter_ = 10;
					break;
				case 1:		// auto
					partyCommand_ = kPartyCommandAuto;
					decided_ = true;
					break;
				case 2:		// escape
					partyCommand_ = kPartyCommandEscape;
					decided_ = true;
					break;
				}
			}
			break;
		case kPageCommand:
			if (canceled) {
				backPlayer(selectWindows_[kPageChara]->cursor() - 1);
			} else if (selected) {
				attackType_ = cursor;
				attackId_ = 0;
				switch (cursor) {
				case 0:		// attack
					setPage(kPageTarget);
					break;
				case 1:		// skill
					setPage(kPageSkill);
					break;
				case 2:		// defence
					{
						AttackInfo info;
						info.target = NULL;
						info.type = kAttackTypeDefence;
						info.id = 0;
						battle_.players()[selectWindows_[kPageChara]->cursor()]->setAttackInfo(info);
						nextPlayer(selectWindows_[kPageChara]->cursor() + 1);
					}
					break;
				case 3:		// item
					setPage(kPageItem);
					break;
				}
			}
			break;
		case kPageSkill:
			if (canceled) {
				setPage(kPageCommand);
			} else if (selected) {
				if (!selectIdList_.empty()) {
					attackId_ = selectIdList_[cursor];
					const rpg2k::structure::Array1D& skill = battle_.project().getLDB().skill()[attackId_];
					GameBattlePlayer* player = battle_.players()[selectWindows_[kPageChara]->cursor()];
					if (player->status().mp() >= skill[11].to<int>()) {
						switch (skill[12].to<int>()) {
						case 0: // to single enemy
							setPage(kPageTarget);
							break;
						case 2: // to single member
							setPage(kPageTargetFriends);
							break;
						default:
							{
								AttackInfo info;
								info.target = NULL;
								info.type = (AttackType)attackType_;
								info.id = attackId_;
								player->setAttackInfo(info);
								nextPlayer(selectWindows_[kPageChara]->cursor() + 1);
							}
							break;
						}
					}
				} else {
					setPage(kPageCommand);
				}
			} else {
				if (!selectIdList_.empty()) {
					int skillId = selectIdList_[cursor];
					const rpg2k::structure::Array1D& skill = battle_.project().getLDB().skill()[skillId];
					descriptionWindow_.clearMessages();
					descriptionWindow_.addLine(skill[2].to_string().toSystem());
				}
			}
			break;
		case kPageItem:
			if (canceled) {
				setPage(kPageCommand);
			} else if (selected) {
				if (!selectIdList_.empty()) {
					attackId_ = selectIdList_[cursor];
					const rpg2k::structure::Array1D& item = battle_.project().getLDB().item()[attackId_];
					GameBattlePlayer* player = battle_.players()[selectWindows_[kPageChara]->cursor()];
					switch (item[3].to<int>()) {
					case rpg2k::Item::MEDICINE:
						if (item[31].to<int>() == 0) {
							setPage(kPageTargetFriends);
						} else {
							AttackInfo info;
							info.target = NULL;
							info.type = (AttackType)attackType_;
							info.id = attackId_;
							player->setAttackInfo(info);
							nextPlayer(selectWindows_[kPageChara]->cursor() + 1);
						}
						break;
					case rpg2k::Item::SPECIAL:
						{
							const rpg2k::structure::Array1D& skill = battle_.project().getLDB().skill()[item[53].to<int>()];
							switch (skill[31].to<int>()) {
							case 0:
								setPage(kPageTarget);
								break;
							case 1:
								setPage(kPageTargetFriends);
								break;
							default:
								{
									AttackInfo info;
									info.target = NULL;
									info.type = (AttackType)attackType_;
									info.id = attackId_;
									player->setAttackInfo(info);
									nextPlayer(selectWindows_[kPageChara]->cursor() + 1);
								}
								break;
							}
						}
						break;
					}
				} else {
					setPage(kPageCommand);
				}
			} else {
				if (!selectIdList_.empty()) {
					int itemId = selectIdList_[cursor];
					const rpg2k::structure::Array1D& item = battle_.project().getLDB().item()[itemId];
					descriptionWindow_.clearMessages();
					descriptionWindow_.addLine(item[2].to_string().toSystem());
				}
			}
			break;
		case kPageTarget:
			if (canceled) {
				setPage(oldPage_);
			} else if (selected) {
				AttackInfo info;
				info.target = battle_.enemies()[cursor];
				info.type = (AttackType)attackType_;
				info.id = attackId_;
				battle_.players()[selectWindows_[kPageChara]->cursor()]->setAttackInfo(info);
				nextPlayer(selectWindows_[kPageChara]->cursor() + 1);
			}
			break;
		case kPageTargetFriends:
			if (canceled) {
				setPage(oldPage_);
			} else if (selected) {
				AttackInfo info;
				info.target = battle_.players()[cursor];
				info.type = (AttackType)attackType_;
				info.id = attackId_;
				battle_.players()[selectWindows_[kPageChara]->cursor()]->setAttackInfo(info);
				nextPlayer(selectWindows_[kPageChara]->cursor() + 1);
			}
			break;
		}
	}
}

void GameBattleMenu::render(kuto::Graphics2D& g) const
{
}

void GameBattleMenu::setPage(int newPage)
{
	if (page_ == newPage) return;

	oldPage_ = page_;
	page_ = newPage;

	selectWindows_[page_]->pauseUpdate(false);
	selectWindows_[page_]->pauseDraw(false);
	selectWindows_[page_]->setPauseUpdateCursor(false);
	selectWindows_[page_]->reset();
	switch (page_) {
	case kPageTarget:
		selectWindows_[page_]->resetCursor();
		selectWindows_[page_]->clearMessages();
		for (uint i = 0; i < battle_.enemies().size(); i++) {
			if (!battle_.enemies()[i]->isExcluded())
				selectWindows_[page_]->addLine(battle_.enemies()[i]->name());
		}
		selectWindows_[oldPage_]->setPauseUpdateCursor(true);
		break;
	case kPageTargetFriends:
		selectWindows_[page_]->resetCursor();
		selectWindows_[page_]->clearMessages();
		for (uint i = 0; i < battle_.players().size(); i++) {
			selectWindows_[page_]->addLine(battle_.players()[i]->name());
		}
		selectWindows_[oldPage_]->setPauseUpdateCursor(true);
		break;
	case kPageSkill:
		selectWindows_[page_]->clearMessages();
		selectIdList_.clear();
		{
			int playerIndex = selectWindows_[kPageChara]->cursor();
			const GameBattlePlayer* battlePlayer = battle_.players()[playerIndex];
			const rpg2k::structure::Array2D& skillList = battle_.project().getLDB().skill();
			for (rpg2k::structure::Array2D::ConstIterator it = skillList.begin(); it != skillList.end(); ++it) {
				if( !it->second->exists() ) continue;

				if (battlePlayer->status().isLearnedSkill(it->first)) {
					if ((*it->second)[8].to<int>() == rpg2k::Skill::NORMAL
					|| ((*it->second)[8].to<int>() == rpg2k::Skill::SWITCH && (*it->second)[19].to<bool>())) {
						selectWindows_[page_]->addLine((*it->second)[1].to_string().toSystem());
						selectIdList_.push_back(it->first);
					}
				}
			}
		}
		descriptionWindow_.freeze(false);
		selectWindows_[oldPage_]->pauseUpdate();
		selectWindows_[oldPage_]->pauseDraw(true);
		break;
	case kPageItem:
		selectWindows_[page_]->clearMessages();
		selectIdList_.clear();
		for (int i = 0; i < battle_.project().getLSD().item().rbegin()->first+1; i++) {
			if (battle_.project().getLSD().itemNum(i) > 0) {
				const rpg2k::structure::Array1D& item = battle_.project().getLDB().item()[i];
				if (item[3].to<int>() == rpg2k::Item::MEDICINE
				||  item[3].to<int>() == rpg2k::Item::SPECIAL) {
					selectWindows_[page_]->addLine(item[1].to_string().toSystem());
					selectIdList_.push_back(i);
				}
			}
		}
		descriptionWindow_.freeze(false);
		selectWindows_[oldPage_]->pauseUpdate();
		selectWindows_[oldPage_]->pauseDraw(true);
		break;
	default:
		descriptionWindow_.freeze();
		selectWindows_[oldPage_]->pauseUpdate();
		selectWindows_[oldPage_]->pauseDraw(true);
		break;
	}
}

void GameBattleMenu::changePlayer(int index)
{
	const rpg2k::model::DataBase& ldb = battle_.project().getLDB();

	selectWindows_[oldPage_]->pauseUpdate();
	selectWindows_[oldPage_]->pauseDraw(true);
	selectWindows_[kPageCommand]->resetCursor();
	selectWindows_[kPageSkill]->resetCursor();
	selectWindows_[kPageItem]->resetCursor();
	selectWindows_[kPageTarget]->resetCursor();
	selectWindows_[kPageTargetFriends]->resetCursor();
	selectWindows_[kPageChara]->setCursor(index);
	selectWindows_[kPageCommand]->reset();

	selectWindows_[kPageCommand]->clearMessages();
	selectWindows_[kPageCommand]->addLine(ldb.vocabulary(104).toSystem());
	if (ldb.character()[battle_.players()[index]->playerID()][66].to<bool>())
		selectWindows_[kPageCommand]->addLine(
		ldb.character()[battle_.players()[index]->playerID()][67].to_string().toSystem());
	else
		selectWindows_[kPageCommand]->addLine(ldb.vocabulary(107).toSystem());
	selectWindows_[kPageCommand]->addLine(ldb.vocabulary(105).toSystem());
	selectWindows_[kPageCommand]->addLine(ldb.vocabulary(106).toSystem());

	setPage(kPageCommand);
}

void GameBattleMenu::nextPlayer(int nextIndex)
{
	if (nextIndex < (int)battle_.players().size()) {
		// next character
		GameBattlePlayer* player = battle_.players()[nextIndex];
		if (!player->isActive() || player->isExecAI())
			nextPlayer(nextIndex + 1);
		else
			changePlayer(nextIndex);
	} else {
		// end
		decided_ = true;
	}
}

void GameBattleMenu::backPlayer(int backIndex)
{
	if (backIndex >= 0) {
		// next character
		GameBattlePlayer* player = battle_.players()[backIndex];
		if (!player->isActive() || player->isExecAI())
			backPlayer(backIndex - 1);
		else
			changePlayer(backIndex);
	} else {
		// end
		selectWindows_[kPageTop]->pauseUpdate(false);
		selectWindows_[kPageTop]->pauseDraw(false);
		selectWindows_[kPageTop]->setPauseUpdateCursor(false);
		selectWindows_[kPageTop]->reset();
		animationCounter_ = 10;
	}
}

void GameBattleMenu::reset()
{
	page_ = kPageTop;
	oldPage_ = kPageTop;
	animationCounter_ = 0;
	decided_ = false;
	for (int i = 0; i < kPageMax; i++) {
		selectWindows_[i]->reset();
		selectWindows_[i]->resetCursor();
		selectWindows_[i]->pauseUpdate();
		selectWindows_[i]->pauseDraw(true);
		selectWindows_[i]->setPauseUpdateCursor(true);
	}

	selectWindows_[kPageTop]->setPosition(kuto::Vector2(0.f, 160.f));
	selectWindows_[kPageChara]->setPosition(kuto::Vector2(70.f, 160.f));
	selectWindows_[kPageCommand]->setPosition(kuto::Vector2(320.f, 160.f));
	selectWindows_[kPageTop]->pauseUpdate(false);
	selectWindows_[kPageTop]->pauseDraw(false);
	selectWindows_[kPageTop]->setPauseUpdateCursor(false);
	selectWindows_[kPageChara]->pauseUpdate(false);
	selectWindows_[kPageChara]->pauseDraw(false);
	descriptionWindow_.freeze();

	updateCharaWindow();
}
