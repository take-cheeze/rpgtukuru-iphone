/**
 * @file
 * @brief Game Battle Menu
 * @author project.kuto
 */

#include <kuto/kuto_stringstream.h>
#include <kuto/kuto_render_manager.h>
#include "game_battle_menu.h"
#include "game_select_window.h"
#include "game_message_window.h"
#include "game_battle.h"
#include "game_battle_chara.h"
#include "game_inventory.h"

using namespace rpg2kLib::model;
using namespace rpg2kLib::structure;


GameBattleMenu::GameBattleMenu(GameBattle* battle)
: kuto::Task(battle), gameBattle_(battle)
, page_(kPageTop), animationCounter_(0), decided_(false), partyCommand_(kPartyCommandManual)
{
	oldPage_ = page_;
	for (int i = 0; i < kPageMax; i++) {
		selectWindows_[i] = GameSelectWindow::createTask(this, gameBattle_->getGameSystem());
		selectWindows_[i]->pauseUpdate(true);
		selectWindows_[i]->pauseDraw(true);
		selectWindows_[i]->setPauseUpdateCursor(true);
		selectWindows_[i]->setPriority(3.f);
		selectWindows_[i]->setAutoClose(false);
	}
	const DataBase& ldb = gameBattle_->getGameSystem().getRpgLdb();
	const Array1D & voc = ldb.getVocabulary();
	selectWindows_[kPageTop]->addMessage(voc[0x65]);
	selectWindows_[kPageTop]->addMessage(voc[0x66]);
	selectWindows_[kPageTop]->addMessage(voc[0x67]);
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
	
	descriptionWindow_ = GameMessageWindow::createTask(this, gameBattle_->getGameSystem());
	descriptionWindow_->setPosition(kuto::Vector2(0.f, 0.f));
	descriptionWindow_->setSize(kuto::Vector2(320.f, 32.f));
	descriptionWindow_->setEnableClick(false);
	descriptionWindow_->setUseAnimation(false);
	
	selectWindows_[kPageTop]->pauseUpdate(false);
	selectWindows_[kPageTop]->pauseDraw(false);
	selectWindows_[kPageTop]->setPauseUpdateCursor(false);
	selectWindows_[kPageChara]->pauseUpdate(false);
	selectWindows_[kPageChara]->pauseDraw(false);
	descriptionWindow_->pauseUpdate(true);
}

void GameBattleMenu::updateCharaWindow()
{
	std::ostringstream ss;
	initStringStream(ss);
	const DataBase& ldb = gameBattle_->getGameSystem().getRpgLdb();
	const Array1D & voc = ldb.getVocabulary();

	selectWindows_[kPageChara]->clearMessages();
	for (u32 i = 0; i < gameBattle_->getPlayers().size(); i++) {
		GameBattleChara* chara = gameBattle_->getPlayers()[i];
		int badConditionId = chara->getWorstBadConditionId(false);

		ss.str("");
		ss << chara->getName()
			<< " " << ( (badConditionId == 0) ? voc[0x7e].get_string() : ldb.getCondition()[badConditionId][1].get_string() )
			<< " " << voc[0x81].get_string()
				<< std::setw(3) << chara->getStatus().getHp() << "/"
				<< std::setw(3) << chara->getStatus().getBaseStatus().maxHP
			<< " " << voc[0x82].get_string()
				<< std::setw(3) << chara->getStatus().getMp() << "/"
				<< std::setw(3) << chara->getStatus().getBaseStatus().maxMP;
		selectWindows_[kPageChara]->addMessage( ss.str() );
	}
}

bool GameBattleMenu::initialize()
{
	if (isInitializedChildren()) {
		descriptionWindow_->pauseUpdate(false);
		descriptionWindow_->freeze(true);
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
						gameBattle_->getPlayers()[selectWindows_[kPageChara]->cursor()]->setAttackInfo(info);
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
					const Array1D& skill = gameBattle_->getGameSystem().getRpgLdb().getSkill()[attackId_];
					GameBattlePlayer* player = gameBattle_->getPlayers()[selectWindows_[kPageChara]->cursor()];
					if ( player->getStatus().getMp() >= skill[11].get_int() ) {
						switch ( skill[12].get_int() ) {
						case DataBase::kSkillScopeEnemySingle:
							setPage(kPageTarget);
							break;
						case DataBase::kSkillScopeFriendSingle:
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
					descriptionWindow_->clearMessages();
					descriptionWindow_->addMessage(
						gameBattle_->getGameSystem().
							getRpgLdb().getSkill()[ selectIdList_[cursor] ][2]
					);
				}
			}
			break;
		case kPageItem:
			if (canceled) {
				setPage(kPageCommand);
			} else if (selected) {
				if (!selectIdList_.empty()) {
					attackId_ = selectIdList_[cursor];
					const Array1D& item = gameBattle_->getGameSystem().getRpgLdb().getItem()[attackId_];
					GameBattlePlayer* player = gameBattle_->getPlayers()[selectWindows_[kPageChara]->cursor()];
					switch ( item[3].get_int() ) {
					case DataBase::kItemTypeMedicine:
						if ( item[31].get_int() == DataBase::kItemScopeSingle) {
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
					case DataBase::kItemTypeSpecial:
						{
							const Array1D& skill =
								gameBattle_->getGameSystem().getRpgLdb().getSkill()[ item[53].get_uint() ];
							switch ( skill[12].get_int() ) {
							case DataBase::kSkillScopeEnemySingle:
								setPage(kPageTarget);
								break;
							case DataBase::kSkillScopeFriendSingle:
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
					descriptionWindow_->clearMessages();
					descriptionWindow_->addMessage(
						gameBattle_->getGameSystem().
							getRpgLdb().getItem()[ selectIdList_[cursor] ][2]
					);
				}
			}
			break;
		case kPageTarget:
			if (canceled) {
				setPage(oldPage_);
			} else if (selected) {
				AttackInfo info;
				info.target = gameBattle_->getEnemies()[cursor];
				info.type = (AttackType)attackType_;
				info.id = attackId_;
				gameBattle_->getPlayers()[selectWindows_[kPageChara]->cursor()]->setAttackInfo(info);
				nextPlayer(selectWindows_[kPageChara]->cursor() + 1);
			}
			break;
		case kPageTargetFriends:
			if (canceled) {
				setPage(oldPage_);
			} else if (selected) {
				AttackInfo info;
				info.target = gameBattle_->getPlayers()[cursor];
				info.type = (AttackType)attackType_;
				info.id = attackId_;
				gameBattle_->getPlayers()[selectWindows_[kPageChara]->cursor()]->setAttackInfo(info);
				nextPlayer(selectWindows_[kPageChara]->cursor() + 1);
			}
			break;
		}
	}
}

void GameBattleMenu::draw()
{
	kuto::RenderManager::instance()->addRender(this, kuto::LAYER_2D_OBJECT, 0.f);
}

void GameBattleMenu::render()
{
}

void GameBattleMenu::setPage(int newPage)
{
	if (page_ == newPage)
		return;

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
		for (u32 i = 0; i < gameBattle_->getEnemies().size(); i++) {
			if (!gameBattle_->getEnemies()[i]->isExcluded())
				selectWindows_[page_]->addMessage(gameBattle_->getEnemies()[i]->getName());
		}
		selectWindows_[oldPage_]->setPauseUpdateCursor(true);
		break;
	case kPageTargetFriends:
		selectWindows_[page_]->resetCursor();
		selectWindows_[page_]->clearMessages();
		for (u32 i = 0; i < gameBattle_->getPlayers().size(); i++) {
			selectWindows_[page_]->addMessage(gameBattle_->getPlayers()[i]->getName());
		}
		selectWindows_[oldPage_]->setPauseUpdateCursor(true);
		break;
	case kPageSkill:
		selectWindows_[page_]->clearMessages();
		selectIdList_.clear();
		{
			int playerIndex = selectWindows_[kPageChara]->cursor();
			const GameBattlePlayer* battlePlayer = gameBattle_->getPlayers()[playerIndex];
			const Array2D& skillList = gameBattle_->getGameSystem().getRpgLdb().getSkill();

			for(Array2D::Iterator it = skillList.begin(); it != skillList.end(); ++it) {
				if ( battlePlayer->getStatus().isLearnedSkill( it.first() ) ) {
					const Array1D& skill = it.second();
					int type = skill[8];

					if (
						(type == DataBase::kSkillTypeNormal) ||
						(type == DataBase::kSkillTypeSwitch && skill[19].get_bool() )
					) {
						selectWindows_[page_]->addMessage(skill[1]);
						selectIdList_.push_back( it.first() );
					}
				}
			}
		}
		descriptionWindow_->freeze(false);
		selectWindows_[oldPage_]->pauseUpdate(true);
		selectWindows_[oldPage_]->pauseDraw(true);
		break;
	case kPageItem:
		selectWindows_[page_]->clearMessages();
		selectIdList_.clear();
		for (u32 i = 0; i < gameBattle_->getGameSystem().getInventory()->getItemList().size(); i++) {
			if (gameBattle_->getGameSystem().getInventory()->getItemList()[i] > 0) {
				const Array1D& item = gameBattle_->getGameSystem().getRpgLdb().getItem()[i];
				int type = item[3];

				if (
					(type == DataBase::kItemTypeMedicine) ||
					(type == DataBase::kItemTypeSpecial )
				) {
					selectWindows_[page_]->addMessage(item[1]);
					selectIdList_.push_back(i);
				}
			}
		}
		descriptionWindow_->freeze(false);
		selectWindows_[oldPage_]->pauseUpdate(true);
		selectWindows_[oldPage_]->pauseDraw(true);
		break;
	default:
		descriptionWindow_->freeze(true);
		selectWindows_[oldPage_]->pauseUpdate(true);
		selectWindows_[oldPage_]->pauseDraw(true);
		break;
	}
}

void GameBattleMenu::changePlayer(int index)
{
	const DataBase& ldb = gameBattle_->getGameSystem().getRpgLdb();
	const Array1D & voc = ldb.getVocabulary();
	
	selectWindows_[oldPage_]->pauseUpdate(true);
	selectWindows_[oldPage_]->pauseDraw(true);
	selectWindows_[kPageCommand]->resetCursor();
	selectWindows_[kPageSkill]->resetCursor();
	selectWindows_[kPageItem]->resetCursor();
	selectWindows_[kPageTarget]->resetCursor();
	selectWindows_[kPageTargetFriends]->resetCursor();
	selectWindows_[kPageChara]->setCursor(index);
	selectWindows_[kPageCommand]->reset();

	selectWindows_[kPageCommand]->clearMessages();
	selectWindows_[kPageCommand]->addMessage(voc[0x68]);

	const Array1D& target = ldb.getCharacter()[index];
	selectWindows_[kPageCommand]->addMessage(target[66].get_bool() ? target[67] : voc[0x6b]);

	selectWindows_[kPageCommand]->addMessage(voc[0x69]);
	selectWindows_[kPageCommand]->addMessage(voc[0x6a]);
	
	setPage(kPageCommand);
}

void GameBattleMenu::nextPlayer(int nextIndex)
{
	if ( (uint)nextIndex < gameBattle_->getPlayers().size() ) {
		// next character
		GameBattlePlayer* player = gameBattle_->getPlayers()[nextIndex];
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
		GameBattlePlayer* player = gameBattle_->getPlayers()[backIndex];
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
		selectWindows_[i]->pauseUpdate(true);
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
	descriptionWindow_->freeze(true);
	
	updateCharaWindow();
}


