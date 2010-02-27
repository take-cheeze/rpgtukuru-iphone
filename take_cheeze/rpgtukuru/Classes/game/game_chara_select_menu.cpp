/**
 * @file
 * @brief Select Chara Window
 * @author project.kuto
 */

#include <kuto/kuto_render_manager.h>
#include <kuto/kuto_graphics2d.h>
#include <kuto/kuto_stringstream.h>
#include <kuto/kuto_virtual_pad.h>
#include <kuto/kuto_utility.h>
#include "game_chara_select_menu.h"
#include "game_field.h"
#include "game_player.h"

using namespace rpg2kLib::model;
using namespace rpg2kLib::structure;


GameCharaSelectMenu::GameCharaSelectMenu(kuto::Task* parent, GameField* gameField)
: GameSelectWindow(parent, gameField->getGameSystem())
, gameField_(gameField), useFullSelectKey_(false)
{
	faceEnable_ = true;
	rowHeight_ = 48.f;
	lineSpace_ = 10.f;
}

void GameCharaSelectMenu::update()
{
	clearMessages();
	for (u32 i = 0; i < gameField_->getPlayers().size(); i++) {
		addMessage("");
	}
	switch (state_) {
	case kStateLoop:
		if (!pauseUpdateCursor_) {
			if (useFullSelectKey_) {
				kuto::VirtualPad* virtualPad = kuto::VirtualPad::instance();
				if (virtualPad->repeat(kuto::VirtualPad::KEY_LEFT)) {
					fullSelect_ = false;
				}
				if (virtualPad->repeat(kuto::VirtualPad::KEY_RIGHT)) {
					fullSelect_ = true;
				}
			}
		}
		break;
	default: break;
	}
	GameSelectWindow::update();
}

void GameCharaSelectMenu::render()
{
	renderFrame();
	
	if (showCursor_)
		renderSelectCursor();
	
	for (u32 i = 0; i < gameField_->getPlayers().size(); i++) {
		renderPlayerInfo(i);
	}
	
	if (showCursor_) {
		int rowSize = (int)(size_.y / rowHeight_);
		if (rowSize * columnSize_ + scrollPosition_ * columnSize_ < (int)messages_.size()) {
			renderDownCursor();
		}
		if (scrollPosition_ > 0) {
			renderUpCursor();
		}
	}
}

void GameCharaSelectMenu::renderPlayerInfo(int index)
{
	GamePlayer* gamePlayer = gameField_->getPlayers()[index];
	const DataBase& ldb = gameField_->getGameSystem().getRpgLdb();
	const Array1D& voc = ldb.getVocabulary();
	const Array1D& player = ldb.getCharacter()[gamePlayer->getPlayerId()];
	std::ostringstream ss;
	initStringStream(ss);
	// face
	kuto::Vector2 facePos(8.f + position_.x, index * 58.f + 8.f + position_.y);
	gameField_->getPlayers()[index]->renderFace(facePos);
	// status
	kuto::Graphics2D* g = kuto::RenderManager::instance()->getGraphics2D();
	const kuto::Color color(1.f, 1.f, 1.f, 1.f);
	kuto::Vector2 windowPosition(position_.x + 64.f, position_.y);
	const GameCharaStatus::BadConditionList& badConditions = gamePlayer->getStatus().getBadConditions();
	const char* conditionStr = NULL;
	if (badConditions.empty()) {
		conditionStr = voc[0x7e].get_string().c_str();
	} else {
		GameCharaStatus::BadCondition cond = badConditions[0];
		for (u32 i = 1; i < badConditions.size(); i++) {
			if ( ldb.getCondition()[badConditions[i].id][4].get_int() > ldb.getCondition()[cond.id][4].get_int() ) {
				cond = badConditions[i];
			}
		}
		conditionStr = ldb.getCondition()[cond.id][1].get_string().c_str();
	}
	if (size_.x < 200.f) {	// short version
		kuto::Vector2 pos = windowPosition;
		pos.y += (rowHeight_ + lineSpace_) * index + 8.f + 2.f;
		g->drawText(player[1].get_string().c_str(), pos, color, fontSize_, kuto::Font::TYPE_NORMAL);

		pos = windowPosition;
		pos.y += (rowHeight_ + lineSpace_) * index + 16.f + 8.f;
		g->drawText(voc[0x80].get_string().c_str(), pos, color, fontSize_, kuto::Font::TYPE_NORMAL);
		pos.x += 12.f;
		ss.str("");
		ss << std::setw(2) << gamePlayer->getStatus().getLevel();
		g->drawText( ss.str().c_str(), pos, color, fontSize_, kuto::Font::TYPE_NORMAL );
		pos.x += 48.f;
		g->drawText( voc[0x81].get_string().c_str(), pos, color, fontSize_, kuto::Font::TYPE_NORMAL );
		pos.x += 12.f;
		ss.str("");
		ss
			<< std::setw(3) << gamePlayer->getStatus().getHp() << "/"
			<< std::setw(3) << gamePlayer->getStatus().getBaseStatus().maxHP;
		g->drawText(ss.str().c_str(), pos, color, fontSize_, kuto::Font::TYPE_NORMAL);

		pos = windowPosition;
		pos.y += (rowHeight_ + lineSpace_) * index + 16.f * 2.f + 8.f;
		g->drawText(conditionStr, pos, color, fontSize_, kuto::Font::TYPE_NORMAL);
		pos.x += 60.f;
		g->drawText(voc[0x82].get_string().c_str(), pos, color, fontSize_, kuto::Font::TYPE_NORMAL);
		pos.x += 12.f;
		ss.str("");
		ss
			<< std::setw(3) << gamePlayer->getStatus().getMp() << "/"
			<< std::setw(3) << gamePlayer->getStatus().getBaseStatus().maxMP;
		g->drawText(ss.str().c_str(), pos, color, fontSize_, kuto::Font::TYPE_NORMAL);
	} else {
		kuto::Vector2 pos = windowPosition;
		pos.y += (rowHeight_ + lineSpace_) * index + 8.f + 2.f;
		g->drawText(player[1].get_string().c_str(), pos, color, fontSize_, kuto::Font::TYPE_NORMAL);
		pos.x += 86.f;
		g->drawText(player[2].get_string().c_str(), pos, color, fontSize_, kuto::Font::TYPE_NORMAL);

		pos = windowPosition;
		pos.y += (rowHeight_ + lineSpace_) * index + 16.f + 8.f;
		g->drawText(voc[0x80].get_string().c_str(), pos, color, fontSize_, kuto::Font::TYPE_NORMAL);
		pos.x += 12.f;
		ss.str("");
		ss << std::setw(2) << gamePlayer->getStatus().getLevel();
		g->drawText(ss.str().c_str(), pos, color, fontSize_, kuto::Font::TYPE_NORMAL);
		pos.x += 30.f;
		g->drawText(conditionStr, pos, color, fontSize_, kuto::Font::TYPE_NORMAL);
		pos.x += 50.f;
		g->drawText(voc[0x81].get_string().c_str(), pos, color, fontSize_, kuto::Font::TYPE_NORMAL);
		pos.x += 22.f;
		ss.str("");
		ss
			<< std::setw(3) << gamePlayer->getStatus().getHp() << "/"
			<< std::setw(3) << gamePlayer->getStatus().getBaseStatus().maxHP;
		g->drawText(ss.str().c_str(), pos, color, fontSize_, kuto::Font::TYPE_NORMAL);

		pos = windowPosition;
		pos.y += (rowHeight_ + lineSpace_) * index + 16.f * 2.f + 8.f;
		g->drawText(voc[0x7f].get_string().c_str(), pos, color, fontSize_, kuto::Font::TYPE_NORMAL);
		pos.x += 12.f;
		ss.str("");
		ss
			<< std::setw(6) << gamePlayer->getStatus().getExp() << "/"
			<< std::setw(6) << gamePlayer->getStatus().getNextLevelExp();
		g->drawText(ss.str().c_str(), pos, color, fontSize_, kuto::Font::TYPE_NORMAL);
		pos.x += 80.f;
		g->drawText(voc[0x82].get_string().c_str(), pos, color, fontSize_, kuto::Font::TYPE_NORMAL);
		pos.x += 22.f;
		ss.str("");
		ss
			<< std::setw(3) << gamePlayer->getStatus().getMp() << "/"
			<< std::setw(3) << gamePlayer->getStatus().getBaseStatus().maxMP;
		g->drawText(ss.str().c_str(), pos, color, fontSize_, kuto::Font::TYPE_NORMAL);
	}
}

