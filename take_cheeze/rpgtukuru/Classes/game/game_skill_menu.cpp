/**
 * @file
 * @brief Save Menu
 * @author project.kuto
 */

#include <kuto/kuto_render_manager.h>
#include <kuto/kuto_graphics2d.h>
#include <kuto/kuto_virtual_pad.h>
#include <kuto/kuto_file.h>
#include "game_skill_menu.h"
#include "game_system.h"
#include "game_field.h"
#include "game_select_window.h"
#include "game_message_window.h"
#include "game_chara_status.h"
#include "game_inventory.h"
#include "game_chara_select_menu.h"
#include "game_player.h"


GameSkillMenu::GameSkillMenu(GameField* gameField, GameCharaStatus* charaStatus)
: GameSystemMenuBase(gameField)
, charaStatus_(charaStatus)
{
	skillMenu_ = GameSelectWindow::createTask(this, gameField_->getGameSystem());
	skillMenu_->pauseUpdate(true);
	skillMenu_->setPosition(kuto::Vector2(0.f, 64.f));
	skillMenu_->setSize(kuto::Vector2(320.f, 176.f));
	skillMenu_->setAutoClose(false);
	skillMenu_->setColumnSize(2);

	descriptionWindow_ = GameMessageWindow::createTask(this, gameField_->getGameSystem());
	descriptionWindow_->pauseUpdate(true);
	descriptionWindow_->setPosition(kuto::Vector2(0.f, 0.f));
	descriptionWindow_->setSize(kuto::Vector2(320.f, 32.f));
	descriptionWindow_->setEnableClick(false);
	descriptionWindow_->setUseAnimation(false);

	charaStatusWindow_ = GameMessageWindow::createTask(this, gameField_->getGameSystem());
	charaStatusWindow_->pauseUpdate(true);
	charaStatusWindow_->setPosition(kuto::Vector2(0.f, 32.f));
	charaStatusWindow_->setSize(kuto::Vector2(320.f, 32.f));
	charaStatusWindow_->setEnableClick(false);
	charaStatusWindow_->setUseAnimation(false);

	charaMenu_ = GameCharaSelectMenu::createTask(this, gameField_);
	charaMenu_->pauseUpdate(true);
	charaMenu_->setPosition(kuto::Vector2(136.f, 0.f));
	charaMenu_->setSize(kuto::Vector2(184.f, 240.f));
	charaMenu_->setAutoClose(false);
	
	skillNameWindow_ = GameMessageWindow::createTask(this, gameField_->getGameSystem());
	skillNameWindow_->pauseUpdate(true);
	skillNameWindow_->setPosition(kuto::Vector2(0.f, 0.f));
	skillNameWindow_->setSize(kuto::Vector2(136.f, 32.f));
	skillNameWindow_->setEnableClick(false);
	skillNameWindow_->setUseAnimation(false);
	
	mpWindow_ = GameMessageWindow::createTask(this, gameField_->getGameSystem());
	mpWindow_->pauseUpdate(true);
	mpWindow_->setPosition(kuto::Vector2(0.f, 32.f));
	mpWindow_->setSize(kuto::Vector2(136.f, 32.f));
	mpWindow_->setEnableClick(false);
	mpWindow_->setUseAnimation(false);
}

GameSkillMenu::~GameSkillMenu()
{
}

bool GameSkillMenu::initialize()
{
	if (isInitializedChildren()) {
		skillMenu_->pauseUpdate(false);
		descriptionWindow_->pauseUpdate(false);
		charaStatusWindow_->pauseUpdate(false);
		charaMenu_->pauseUpdate(false);
		charaMenu_->freeze(true);
		skillNameWindow_->pauseUpdate(false);
		skillNameWindow_->freeze(true);
		mpWindow_->pauseUpdate(false);
		mpWindow_->freeze(true);
		start();
		return true;
	}
	return false;
}

void GameSkillMenu::update()
{
	const DataBase& ldb = gameField_->getGameSystem().getRpgLdb();
	switch (state_) {
	case kStateSkill:
		updateDiscriptionMessage();
		if (skillMenu_->selected()) {
			const DataBase::Skill& skill = ldb.saSkill[skillList_[skillMenu_->cursor()]];
			switch (skill.type) {
			case DataBase::kSkillTypeNormal:
				switch (skill.scope) {
				case DataBase::kSkillScopeMyself:
				case DataBase::kSkillScopeFriendSingle:
				case DataBase::kSkillScopeFriendAll:
					setState(kStateChara);
					break;
				default:
					skillMenu_->reset();
					break;
				}
				break;
			case DataBase::kSkillTypeTeleport:
			case DataBase::kSkillTypeEscape:
				skillMenu_->reset();
				break;
			case DataBase::kSkillTypeSwitch:
				if (skill.useField) {
					gameField_->getGameSystem().setSwitch(skill.onSwitch, true);
					setState(kStateSystemMenuEnd);
				} else {
					skillMenu_->reset();
				}
				break;
			default:
				skillMenu_->reset();
				break;
			}
		} else if (skillMenu_->canceled()) {
			setState(kStateNone);
		}
		break;
	case kStateChara:
		updateDiscriptionMessage();
		if (charaMenu_->selected()) {
			const DataBase::Skill& skill = ldb.saSkill[skillList_[skillMenu_->cursor()]];
			if (skill.consumeMPFix <= charaStatus_->getMp()) {
				int playerId = (skill.scope != DataBase::kSkillScopeFriendAll)? gameField_->getPlayers()[charaMenu_->cursor()]->getPlayerId() : 0;
				if (applySkill(skillList_[skillMenu_->cursor()], playerId)) {
					charaStatus_->consumeMp(skill.consumeMPFix);
				}
			}
			charaMenu_->reset();
		} else if (charaMenu_->canceled()) {
			setState(kStateSkill);
		}
		break;
	}
}

bool GameSkillMenu::applySkill(int skillId, int playerId)
{
	kuto::StaticVector<GameCharaStatus*, 4> statusList;
	if (playerId == 0) {
		// party
		for (u32 i = 0; i < gameField_->getPlayers().size(); i++)
			statusList.push_back(&gameField_->getPlayers()[i]->getStatus());
	} else {
		statusList.push_back(&gameField_->getPlayerFromId(playerId)->getStatus());
	}
	bool applyOk = false;
	for (u32 i = 0; i < statusList.size(); i++) {
		if (statusList[i]->applySkill(skillId, charaStatus_)) {
			applyOk = true;
		}
	}
	return applyOk;
}

void GameSkillMenu::setState(int newState)
{
	state_ = newState;
	const DataBase& ldb = gameField_->getGameSystem().getRpgLdb();
	switch (state_) {
	case kStateSkill:
		skillMenu_->freeze(false);
		descriptionWindow_->freeze(false);
		charaStatusWindow_->freeze(false);
		charaMenu_->freeze(true);
		skillNameWindow_->freeze(true);
		mpWindow_->freeze(true);
		updateSkillWindow();
		skillMenu_->reset();
		break;
	case kStateChara:
		skillMenu_->freeze(true);
		descriptionWindow_->freeze(true);
		charaStatusWindow_->freeze(true);
		charaMenu_->freeze(false);
		skillNameWindow_->freeze(false);
		mpWindow_->freeze(false);
		charaMenu_->reset();
		{
			const DataBase::Skill& skill = ldb.saSkill[skillList_[skillMenu_->cursor()]];
			charaMenu_->setFullSelect(skill.scope == DataBase::kSkillScopeFriendAll);
		}
		break;
	}
}

void GameSkillMenu::updateDiscriptionMessage()
{
	const DataBase& ldb = gameField_->getGameSystem().getRpgLdb();
	descriptionWindow_->clearMessages();
	charaStatusWindow_->clearMessages();
	skillNameWindow_->clearMessages();
	mpWindow_->clearMessages();
	switch (state_) {
	case kStateSkill:
		if (!skillList_.empty() && skillList_[skillMenu_->cursor()] > 0)
			descriptionWindow_->addMessage(ldb.saSkill[skillList_[skillMenu_->cursor()]].explain);
		{
			const DataBase::Player& player = ldb.saPlayer[charaStatus_->getCharaId()];
			const GameCharaStatus::BadConditionList& badConditions = charaStatus_->getBadConditions();
			const char* conditionStr = NULL;
			if (badConditions.empty()) {
				conditionStr = ldb.term.param.condition.c_str();
			} else {
				GameCharaStatus::BadCondition cond = badConditions[0];
				for (u32 i = 1; i < badConditions.size(); i++) {
					if (ldb.saCondition[badConditions[i].id].priority > ldb.saCondition[cond.id].priority) {
						cond = badConditions[i];
					}
				}
				conditionStr = ldb.saCondition[cond.id].name.c_str();
			}
			char temp[256];
			sprintf(temp, "%s  %s%2d  %s  %s%3d/%3d  %s%3d/%3d", player.name.c_str(),
				ldb.term.param.levelShort.c_str(),charaStatus_->getLevel(), conditionStr,
				ldb.term.param.hpShort.c_str(), charaStatus_->getHp(), (int)charaStatus_->getBaseStatus().maxHP,
				ldb.term.param.mpShort.c_str(), charaStatus_->getMp(), (int)charaStatus_->getBaseStatus().maxMP);
			charaStatusWindow_->addMessage(temp);
		}
		break;
	case kStateChara:
		if (!skillList_.empty() && skillList_[skillMenu_->cursor()] > 0) {
			const DataBase::Skill& skill = ldb.saSkill[skillList_[skillMenu_->cursor()]];
			skillNameWindow_->addMessage(skill.name);
			char temp[256];
			sprintf(temp, "%s %d", ldb.term.param.consumeMp.c_str(), skill.consumeMPFix);
			mpWindow_->addMessage(temp);
		}
		break;
	}
}

void GameSkillMenu::updateSkillWindow()
{
	const DataBase& ldb = gameField_->getGameSystem().getRpgLdb();
	skillList_.clear();
	skillMenu_->clearMessages();
	char temp[256];
	for (u32 i = 1; i < ldb.saSkill.GetSize(); i++) {
		if (charaStatus_->isLearnedSkill(i)) {
			skillList_.push_back(i);
			sprintf(temp, "%s - %2d", ldb.saSkill[i].name.c_str(), ldb.saSkill[i].consumeMPFix);
			skillMenu_->addMessage(temp);
		}
	}
}

void GameSkillMenu::start()
{
	freeze(false);
	setState(kStateSkill);
}

void GameSkillMenu::draw()
{
	kuto::RenderManager::instance()->addRender(this, kuto::LAYER_2D_OBJECT, 20.f);
}

void GameSkillMenu::render()
{
}
