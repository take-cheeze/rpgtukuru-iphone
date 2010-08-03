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
	const rpg2k::model::DataBase& ldb = gameField_->getGameSystem().getLDB();
	switch (state_) {
	case kStateSkill:
		updateDiscriptionMessage();
		if (skillMenu_->selected()) {
			const rpg2k::structure::Array1D& skill = ldb.skill()[skillList_[skillMenu_->cursor()]];
			switch (skill[8].get<int>()) {
			case rpg2k::Skill::NORMAL:
				switch (skill[12].get<int>()) {
				case 2: // self
				case 3: // single member
				case 4: //  all member
					setState(kStateChara);
					break;
				default:
					skillMenu_->reset();
					break;
				}
				break;
			case rpg2k::Skill::TELEPORT:
			case rpg2k::Skill::ESCAPE:
				skillMenu_->reset();
				break;
			case rpg2k::Skill::SWITCH:
				if (skill[18].get<bool>()) {
					gameField_->getGameSystem().getLSD().setFlag(skill[13].get<int>(), true);
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
			const rpg2k::structure::Array1D& skill = ldb.skill()[skillList_[skillMenu_->cursor()]];
			if (skill[11].get<int>() <= charaStatus_->getMp()) {
				int playerId = (skill[12].get<int>() != 4)? gameField_->getPlayers()[charaMenu_->cursor()]->getPlayerId() : 0;
				if (applySkill(skillList_[skillMenu_->cursor()], playerId)) {
					charaStatus_->consumeMp(skill[11].get<int>());
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
		for (uint i = 0; i < gameField_->getPlayers().size(); i++)
			statusList.push_back(&gameField_->getPlayers()[i]->getStatus());
	} else {
		statusList.push_back(&gameField_->getPlayerFromId(playerId)->getStatus());
	}
	bool applyOk = false;
	for (uint i = 0; i < statusList.size(); i++) {
		if (statusList[i]->applySkill(skillId, charaStatus_)) {
			applyOk = true;
		}
	}
	return applyOk;
}

void GameSkillMenu::setState(int newState)
{
	state_ = newState;
	const rpg2k::model::DataBase& ldb = gameField_->getGameSystem().getLDB();
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
			const rpg2k::structure::Array1D& skill = ldb.skill()[skillList_[skillMenu_->cursor()]];
			charaMenu_->setFullSelect(skill[12].get<int>() == 4);
		}
		break;
	}
}

void GameSkillMenu::updateDiscriptionMessage()
{
	const rpg2k::model::DataBase& ldb = gameField_->getGameSystem().getLDB();
	descriptionWindow_->clearMessages();
	charaStatusWindow_->clearMessages();
	skillNameWindow_->clearMessages();
	mpWindow_->clearMessages();
	switch (state_) {
	case kStateSkill:
		if (!skillList_.empty() && skillList_[skillMenu_->cursor()] > 0)
			descriptionWindow_->addLine(ldb.skill()[skillList_[skillMenu_->cursor()]][2].get_string());
		{
			const GameCharaStatus::BadConditionList& badConditions = charaStatus_->getBadConditions();
			const char* conditionStr = NULL;
			if (badConditions.empty()) {
				conditionStr = ldb.vocabulary(126).c_str();
			} else {
				GameCharaStatus::BadCondition cond = badConditions[0];
				for (uint i = 1; i < badConditions.size(); i++) {
					if (ldb.condition()[badConditions[i].id][4].get<int>() > ldb.condition()[cond.id][4].get<int>()) {
						cond = badConditions[i];
					}
				}
				conditionStr = ldb.condition()[cond.id][1].get_string().c_str();
			}
			char temp[256];
			sprintf(temp, "%s  %s%2d  %s  %s%3d/%3d  %s%3d/%3d", gameField_->getGameSystem().name(charaStatus_->getCharaId()).c_str(),
				ldb.vocabulary(128).c_str(),charaStatus_->getLevel(), conditionStr,
				ldb.vocabulary(129).c_str(), charaStatus_->getHp(), (int)charaStatus_->getBaseStatus()[rpg2k::Param::HP],
				ldb.vocabulary(130).c_str(), charaStatus_->getMp(), (int)charaStatus_->getBaseStatus()[rpg2k::Param::MP]);
			charaStatusWindow_->addLine(temp);
		}
		break;
	case kStateChara:
		if (!skillList_.empty() && skillList_[skillMenu_->cursor()] > 0) {
			const rpg2k::structure::Array1D& skill = ldb.skill()[skillList_[skillMenu_->cursor()]];
			skillNameWindow_->addLine(skill[1].get_string());
			char temp[256];
			sprintf(temp, "%s %d", ldb.vocabulary(131).c_str(), skill[11].get<int>());
			mpWindow_->addLine(temp);
		}
		break;
	}
}

void GameSkillMenu::updateSkillWindow()
{
	const rpg2k::model::DataBase& ldb = gameField_->getGameSystem().getLDB();
	skillList_.clear();
	skillMenu_->clearMessages();
	char temp[256];
	for (uint i = 1; i <= ldb.skill().rend().first(); i++) {
		if (charaStatus_->isLearnedSkill(i)) {
			skillList_.push_back(i);
			sprintf(temp, "%s - %2d", ldb.skill()[i][1].get_string().c_str(), ldb.skill()[i][11].get<int>());
			skillMenu_->addLine(temp);
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
