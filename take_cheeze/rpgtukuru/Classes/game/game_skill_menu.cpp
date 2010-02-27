/**
 * @file
 * @brief Save Menu
 * @author project.kuto
 */

#include <kuto/kuto_render_manager.h>
#include <kuto/kuto_graphics2d.h>
#include <kuto/kuto_virtual_pad.h>
#include <kuto/kuto_file.h>
#include <kuto/kuto_stringstream.h>
#include "game_skill_menu.h"
#include "game_system.h"
#include "game_field.h"
#include "game_select_window.h"
#include "game_message_window.h"
#include "game_chara_status.h"
#include "game_inventory.h"
#include "game_chara_select_menu.h"
#include "game_player.h"

using namespace rpg2kLib::model;
using namespace rpg2kLib::structure;


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
			const Array1D& skill = ldb.getSkill()[skillList_[skillMenu_->cursor()]];

			switch ( skill[8].get_int() ) {
			case DataBase::kSkillTypeNormal:
				switch ( skill[12].get_int() ) {
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
				if ( skill[19].get_bool() ) {
					gameField_->getGameSystem().setSwitch(skill[13], true);
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
			const Array1D& skill = ldb.getSkill()[skillList_[skillMenu_->cursor()]];
			if (skill[11].get_int() <= charaStatus_->getMp()) {
				int playerId = (skill[12].get_int() != DataBase::kSkillScopeFriendAll)? gameField_->getPlayers()[charaMenu_->cursor()]->getPlayerId() : 0;
				if (applySkill(skillList_[skillMenu_->cursor()], playerId)) {
					charaStatus_->consumeMp( skill[11].get_int() );
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
			const Array1D& skill = ldb.getSkill()[skillList_[skillMenu_->cursor()]];
			charaMenu_->setFullSelect(skill[12].get_int() == DataBase::kSkillScopeFriendAll);
		}
		break;
	}
}

void GameSkillMenu::updateDiscriptionMessage()
{
	const DataBase& ldb = gameField_->getGameSystem().getRpgLdb();
	const Array1D& voc = ldb.getVocabulary();

	descriptionWindow_->clearMessages();
	charaStatusWindow_->clearMessages();
	skillNameWindow_->clearMessages();
	mpWindow_->clearMessages();
	switch (state_) {
	case kStateSkill:
		if (!skillList_.empty() && skillList_[skillMenu_->cursor()] > 0)
			descriptionWindow_->addMessage(ldb.getSkill()[skillList_[skillMenu_->cursor()]][2]);
		{
			const Array1D& player = ldb.getCharacter()[charaStatus_->getCharaId()];
			const GameCharaStatus::BadConditionList& badConditions = charaStatus_->getBadConditions();

			const char* condName = NULL;
			if ( badConditions.empty() ) {
				condName = voc[0x7e].get_string().c_str();
			} else {
				GameCharaStatus::BadCondition cond = badConditions[0];
				const Array2D& condList = ldb.getCondition();
				for (u32 i = 1; i < badConditions.size(); i++) {
					if ( condList[badConditions[i].id][4].get_int() > condList[cond.id][4].get_int() ) {
						cond = badConditions[i];
					}
				}
				condName = ldb.getCondition()[cond.id][1].get_string().c_str();
			}

			std::ostringstream ss;
			initStringStream(ss);

			ss.str("");
			ss << player[1].get_string()
				<< " " << voc[0x80].get_string() << std::setw(2) << charaStatus_->getLevel()
				<< "  " << condName
				<< "  " << voc[0x81].get_string()
					<< std::setw(3) << charaStatus_->getHp() << "/"
					<< std::setw(3) << (int)charaStatus_->getBaseStatus().maxHP
				<< "  " << voc[0x82].get_string()
					<< std::setw(3) << charaStatus_->getMp() << "/"
					<< std::setw(3) << (int)charaStatus_->getBaseStatus().maxMP;
			charaStatusWindow_->addMessage( ss.str() );
		}
		break;
	case kStateChara:
		if (!skillList_.empty() && skillList_[skillMenu_->cursor()] > 0) {
			const Array1D& skill = ldb.getSkill()[skillList_[skillMenu_->cursor()]];
			skillNameWindow_->addMessage(skill[1]);

			std::ostringstream ss;
			initStringStream(ss);

			ss.str("");
			ss << voc[0x83].get_string() << " " << std::setw(2) << skill[11].get_int();

			mpWindow_->addMessage( ss.str() );
		}
		break;
	}
}

void GameSkillMenu::updateSkillWindow()
{
	skillList_.clear();
	skillMenu_->clearMessages();

	std::ostringstream ss;
	initStringStream(ss);

	Array2D& skillList = gameField_->getGameSystem().getRpgLdb().getSkill();
	for (Array2D::Iterator it = skillList.begin(); it != skillList.end(); ++it) {
		if ( charaStatus_->isLearnedSkill( it.first() ) ) {
			skillList_.push_back( it.first() );

			ss.str("");
			ss << it.second()[1].get_string().c_str() << " - " << std::setw(2) << it.second()[11].get_int();

			skillMenu_->addMessage( ss.str() );
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
