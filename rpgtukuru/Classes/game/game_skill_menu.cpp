/**
 * @file
 * @brief Save Menu
 * @author project.kuto
 */

#include <kuto/kuto_render_manager.h>
#include <kuto/kuto_graphics2d.h>
#include <kuto/kuto_virtual_pad.h>
#include <kuto/kuto_file.h>
#include <rpg2k/Project.hpp>

#include "game_skill_menu.h"
#include "game_field.h"
#include "game_select_window.h"
#include "game_message_window.h"
#include "game_chara_status.h"
#include "game_chara_select_menu.h"

#include <algorithm>
#include <iterator>


GameSkillMenu::GameSkillMenu(GameField& gameField, int const charaID)
: GameSystemMenuBase(gameField)
, skillMenu_( *addChild(GameSelectWindow::createTask(field_.game())) )
, descriptionWindow_( *addChild(GameMessageWindow::createTask(field_.game())) )
, charaStatusWindow_( *addChild(GameMessageWindow::createTask(field_.game())) )
, charaMenu_( *addChild(kuto::TaskCreatorParam1<GameCharaSelectMenu, GameField&>::createTask(field_)) )
, skillNameWindow_( *addChild(GameMessageWindow::createTask(field_.game())) )
, mpWindow_( *addChild(GameMessageWindow::createTask(field_.game())) )
, charaID_(charaID)
{
	skillMenu_.pauseUpdate();
	skillMenu_.setPosition(kuto::Vector2(0.f, 64.f));
	skillMenu_.setSize(kuto::Vector2(320.f, 176.f));
	skillMenu_.setAutoClose(false);
	skillMenu_.setColumnSize(2);

	descriptionWindow_.pauseUpdate();
	descriptionWindow_.setPosition(kuto::Vector2(0.f, 0.f));
	descriptionWindow_.setSize(kuto::Vector2(320.f, 32.f));
	descriptionWindow_.enableClick(false);
	descriptionWindow_.useAnimation(false);

	charaStatusWindow_.pauseUpdate();
	charaStatusWindow_.setPosition(kuto::Vector2(0.f, 32.f));
	charaStatusWindow_.setSize(kuto::Vector2(320.f, 32.f));
	charaStatusWindow_.enableClick(false);
	charaStatusWindow_.useAnimation(false);

	charaMenu_.pauseUpdate();
	charaMenu_.setPosition(kuto::Vector2(136.f, 0.f));
	charaMenu_.setSize(kuto::Vector2(184.f, 240.f));
	charaMenu_.setAutoClose(false);

	skillNameWindow_.pauseUpdate();
	skillNameWindow_.setPosition(kuto::Vector2(0.f, 0.f));
	skillNameWindow_.setSize(kuto::Vector2(136.f, 32.f));
	skillNameWindow_.enableClick(false);
	skillNameWindow_.useAnimation(false);

	mpWindow_.pauseUpdate();
	mpWindow_.setPosition(kuto::Vector2(0.f, 32.f));
	mpWindow_.setSize(kuto::Vector2(136.f, 32.f));
	mpWindow_.enableClick(false);
	mpWindow_.useAnimation(false);
}

bool GameSkillMenu::initialize()
{
	if (isInitializedChildren()) {
		skillMenu_.pauseUpdate(false);
		descriptionWindow_.pauseUpdate(false);
		charaStatusWindow_.pauseUpdate(false);
		charaMenu_.pauseUpdate(false);
		charaMenu_.freeze();
		skillNameWindow_.pauseUpdate(false);
		skillNameWindow_.freeze();
		mpWindow_.pauseUpdate(false);
		mpWindow_.freeze();
		start();
		return true;
	}
	return false;
}

void GameSkillMenu::update()
{
	const rpg2k::model::DataBase& ldb = field_.project().getLDB();
	switch (state_) {
	case kStateSkill:
		updateDiscriptionMessage();
		if (skillMenu_.selected()) {
			const rpg2k::structure::Array1D& skill = ldb.skill()[skillList_[skillMenu_.cursor()]];
			switch (skill[8].to<int>()) {
			case rpg2k::Skill::NORMAL:
				switch (skill[12].to<int>()) {
				case 2: // self
				case 3: // single member
				case 4: //  all member
					setState(kStateChara);
					break;
				default:
					skillMenu_.reset();
					break;
				}
				break;
			case rpg2k::Skill::TELEPORT:
			case rpg2k::Skill::ESCAPE:
				skillMenu_.reset();
				break;
			case rpg2k::Skill::SWITCH:
				if (skill[18].to<bool>()) {
					field_.project().getLSD().setFlag(skill[13].to<int>(), true);
					setState(kStateSystemMenuEnd);
				} else {
					skillMenu_.reset();
				}
				break;
			default:
				skillMenu_.reset();
				break;
			}
		} else if (skillMenu_.canceled()) {
			setState(kStateNone);
		}
		break;
	case kStateChara:
		updateDiscriptionMessage();
		if (charaMenu_.selected()) {
			const rpg2k::structure::Array1D& skill = ldb.skill()[skillList_[skillMenu_.cursor()]];
			rpg2k::model::Project::Character& c = field_.project().character(charaID_);
			if (skill[11].to<int>() <= c.mp()) {
				int playerId = (skill[12].to<int>() != 4)
					? field_.project().getLSD().member()[charaMenu_.cursor()] : 0;
				if (applySkill(skillList_[skillMenu_.cursor()], playerId)) {
					c.setMP( c.mp() - skill[11].to<int>() );
				}
			}
			charaMenu_.reset();
		} else if (charaMenu_.canceled()) {
			setState(kStateSkill);
		}
		break;
	}
}

bool GameSkillMenu::applySkill(int const skillId, int const playerId)
{
	kuto::StaticVector<unsigned, rpg2k::MEMBER_MAX> target;
	if (playerId == 0) {
		// party
		std::vector<uint16_t> const& mem = field_.project().getLSD().member();
		std::copy( mem.begin(), mem.end(), std::back_inserter(target) );
	} else { target.push_back(playerId); }
	bool applyOk = false;
	for (unsigned i = 0; i < target.size(); i++) {
		/* TODO
		if (target[i]->applySkill(skillId, charaStatus_)) {
			applyOk = true;
		}
		 */
	}
	return applyOk;
}

void GameSkillMenu::setState(int newState)
{
	state_ = newState;
	const rpg2k::model::DataBase& ldb = field_.project().getLDB();
	switch (state_) {
	case kStateSkill:
		skillMenu_.freeze(false);
		descriptionWindow_.freeze(false);
		charaStatusWindow_.freeze(false);
		charaMenu_.freeze();
		skillNameWindow_.freeze();
		mpWindow_.freeze();
		updateSkillWindow();
		skillMenu_.reset();
		break;
	case kStateChara:
		skillMenu_.freeze();
		descriptionWindow_.freeze();
		charaStatusWindow_.freeze();
		charaMenu_.freeze(false);
		skillNameWindow_.freeze(false);
		mpWindow_.freeze(false);
		charaMenu_.reset();
		{
			const rpg2k::structure::Array1D& skill = ldb.skill()[skillList_[skillMenu_.cursor()]];
			charaMenu_.setFullSelect(skill[12].to<int>() == 4);
		}
		break;
	}
}

void GameSkillMenu::updateDiscriptionMessage()
{
	const rpg2k::model::DataBase& ldb = field_.project().getLDB();
	descriptionWindow_.clearMessages();
	charaStatusWindow_.clearMessages();
	skillNameWindow_.clearMessages();
	mpWindow_.clearMessages();
	switch (state_) {
	case kStateSkill:
		if (!skillList_.empty() && skillList_[skillMenu_.cursor()] > 0)
			descriptionWindow_.addLine(ldb.skill()[skillList_[skillMenu_.cursor()]][2].to_string().toSystem());
		{
			rpg2k::model::Project::Character& c = field_.project().character(charaID_);
			const std::vector<uint8_t>& badConditions = c.condition();
			const char* conditionStr = NULL;
			unsigned cond = 0;
			std::vector<uint8_t>::const_iterator cond_it = badConditions.begin();
			while ( ( cond_it = std::find( cond_it, badConditions.end(), true ) ) != badConditions.end() ) {
				unsigned const nextCond = cond_it - badConditions.begin() + 1;
				if (ldb.condition()[nextCond][4].to<int>() > ldb.condition()[cond][4].to<int>()) {
					cond = nextCond;
				}
			}
			if(cond == 0) { conditionStr = ldb.vocabulary(126).toSystem().c_str(); }
			else { conditionStr = ldb.condition()[cond][1].to_string().toSystem().c_str(); }
			char temp[256];
			sprintf(temp, "%s  %s%2d  %s  %s%3d/%3d  %s%3d/%3d"
				, c.name().c_str()
				, ldb.vocabulary(128).toSystem().c_str(), c.level(), conditionStr
				, ldb.vocabulary(129).toSystem().c_str(), c.hp(), (int)c.param(rpg2k::Param::HP)
				, ldb.vocabulary(130).toSystem().c_str(), c.mp(), (int)c.param(rpg2k::Param::MP) );
			charaStatusWindow_.addLine(temp);
		}
		break;
	case kStateChara:
		if (!skillList_.empty() && skillList_[skillMenu_.cursor()] > 0) {
			const rpg2k::structure::Array1D& skill = ldb.skill()[skillList_[skillMenu_.cursor()]];
			skillNameWindow_.addLine(skill[1].to_string().toSystem());
			char temp[256];
			sprintf(temp, "%s %d", ldb.vocabulary(131).toSystem().c_str(), skill[11].to<int>());
			mpWindow_.addLine(temp);
		}
		break;
	}
}

void GameSkillMenu::updateSkillWindow()
{
	const rpg2k::structure::Array2D& skill = field_.project().getLDB().skill();
	skillList_.clear();
	skillMenu_.clearMessages();
	char temp[256];
	std::set<uint16_t> const& learnedSkill = field_.project().character(charaID_).skill();
	for(std::set<uint16_t>::const_iterator i = learnedSkill.begin(); i != learnedSkill.end(); ++i) {
		skillList_.push_back(*i);
		sprintf(temp, "%s - %2d", skill[*i][1].to_string().toSystem().c_str(), skill[*i][11].to<int>());
		skillMenu_.addLine(temp);
	}
}

void GameSkillMenu::start()
{
	freeze(false);
	setState(kStateSkill);
}

void GameSkillMenu::render(kuto::Graphics2D& g) const
{
}
