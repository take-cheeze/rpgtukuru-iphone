/**
 * @file
 * @brief Select Chara Window
 * @author project.kuto
 */

#include <kuto/kuto_render_manager.h>
#include <kuto/kuto_graphics2d.h>
#include <kuto/kuto_virtual_pad.h>
#include <kuto/kuto_utility.h>

#include "game_chara_select_menu.h"
#include "game_field.h"

#include <rpg2k/Project.hpp>


GameCharaSelectMenu::GameCharaSelectMenu(GameField& gameField)
: GameSelectWindow(gameField.game())
, field_(gameField), useFullSelectKey_(false)
{
	faceEnable_ = true;
	rowHeight_ = 48.f;
	lineSpace_ = 10.f;
}

void GameCharaSelectMenu::update()
{
	clearMessages();
	for (uint i = 0; i < field_.project().getLSD().memberNum(); i++) {
		addLine("");
	}
	switch (state_) {
	case kStateLoop:
		if (!pauseUpdateCursor_) {
			if (useFullSelectKey_) {
				kuto::VirtualPad& virtualPad = kuto::VirtualPad::instance();
				if (virtualPad.repeat(kuto::VirtualPad::KEY_LEFT)) {
					fullSelect_ = false;
				}
				if (virtualPad.repeat(kuto::VirtualPad::KEY_RIGHT)) {
					fullSelect_ = true;
				}
			}
		}
		break;
	default: break;
	}
	GameSelectWindow::update();
}

void GameCharaSelectMenu::render(kuto::Graphics2D& g) const
{
	renderFrame(g);

	if (showCursor_) {
		renderSelectCursor(g);
	}

	for (uint i = 0; i < field_.project().getLSD().memberNum(); i++) {
		renderPlayerInfo(g, i);
	}

	if (showCursor_) {
		int rowSize = (int)(size_.y / rowHeight_);
		if (rowSize * columnSize_ + scrollPosition_ * columnSize_ < (int)messages_.size()) {
			renderDownCursor(g);
		}
		if (scrollPosition_ > 0) {
			renderUpCursor(g);
		}
	}
}

void GameCharaSelectMenu::renderPlayerInfo(kuto::Graphics2D& g, int index) const
{
	rpg2k::model::Project const& proj = field_.project();
	rpg2k::model::DataBase const& ldb = proj.getLDB();
	unsigned playerID = proj.getLSD().member(index);
	char temp[256] = "\0";
	// face
	kuto::Vector2 facePos(8.f + position_.x, index * 58.f + 8.f + position_.y);
	// TODO: field_.players()[index]->renderFace(g, facePos);
	// status
	const kuto::Color color(1.f, 1.f, 1.f, 1.f);
	kuto::Vector2 windowPosition(position_.x + 64.f, position_.y);
	rpg2k::model::Project::Character const& character = proj.character(playerID);
	const char* conditionStr = NULL;
	const std::vector<uint8_t>& badConditions = character.condition();
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

	if (size_.x < 200.f) {	// short version
		kuto::Vector2 pos = windowPosition;
		pos.y += (rowHeight_ + lineSpace_) * index + 8.f + 2.f;
		g.drawText(character.name().c_str(), pos, color, fontSize_, kuto::Font::NORMAL);

		pos = windowPosition;
		pos.y += (rowHeight_ + lineSpace_) * index + 16.f + 8.f;
		g.drawText(ldb.vocabulary(128).toSystem().c_str(), pos, color, fontSize_, kuto::Font::NORMAL);
		pos.x += 12.f;
		sprintf(temp, "%2d", character.level());
		g.drawText(temp, pos, color, fontSize_, kuto::Font::NORMAL);
		pos.x += 48.f;
		g.drawText(ldb.vocabulary(129).toSystem().c_str(), pos, color, fontSize_, kuto::Font::NORMAL);
		pos.x += 12.f;
		sprintf(temp, "%3d/%3d", character.hp(), character.param(rpg2k::Param::HP) );
		g.drawText(temp, pos, color, fontSize_, kuto::Font::NORMAL);

		pos = windowPosition;
		pos.y += (rowHeight_ + lineSpace_) * index + 16.f * 2.f + 8.f;
		g.drawText(conditionStr, pos, color, fontSize_, kuto::Font::NORMAL);
		pos.x += 60.f;
		g.drawText(ldb.vocabulary(130).toSystem().c_str(), pos, color, fontSize_, kuto::Font::NORMAL);
		pos.x += 12.f;
		sprintf(temp, "%3d/%3d", character.mp(), character.param(rpg2k::Param::MP));
		g.drawText(temp, pos, color, fontSize_, kuto::Font::NORMAL);
	} else {
		kuto::Vector2 pos = windowPosition;
		pos.y += (rowHeight_ + lineSpace_) * index + 8.f + 2.f;
		g.drawText(character.name().c_str(), pos, color, fontSize_, kuto::Font::NORMAL);
		pos.x += 86.f;
		g.drawText(character.title().c_str(), pos, color, fontSize_, kuto::Font::NORMAL);

		pos = windowPosition;
		pos.y += (rowHeight_ + lineSpace_) * index + 16.f + 8.f;
		g.drawText(ldb.vocabulary(128).toSystem().c_str(), pos, color, fontSize_, kuto::Font::NORMAL);
		pos.x += 12.f;
		sprintf(temp, "%2d", character.level());
		g.drawText(temp, pos, color, fontSize_, kuto::Font::NORMAL);
		pos.x += 30.f;
		g.drawText(conditionStr, pos, color, fontSize_, kuto::Font::NORMAL);
		pos.x += 50.f;
		g.drawText(ldb.vocabulary(129).toSystem().c_str(), pos, color, fontSize_, kuto::Font::NORMAL);
		pos.x += 22.f;
		sprintf(temp, "%3d/%3d", character.hp(), character.param(rpg2k::Param::HP));
		g.drawText(temp, pos, color, fontSize_, kuto::Font::NORMAL);

		pos = windowPosition;
		pos.y += (rowHeight_ + lineSpace_) * index + 16.f * 2.f + 8.f;
		g.drawText(ldb.vocabulary(127).toSystem().c_str(), pos, color, fontSize_, kuto::Font::NORMAL);
		pos.x += 12.f;
		sprintf(temp, "%6d/%6d", character.levelExp(), character.nextLevelExp());
		g.drawText(temp, pos, color, fontSize_, kuto::Font::NORMAL);
		pos.x += 80.f;
		g.drawText(ldb.vocabulary(130).toSystem().c_str(), pos, color, fontSize_, kuto::Font::NORMAL);
		pos.x += 22.f;
		sprintf(temp, "%3d/%3d", character.mp(), character.param(rpg2k::Param::MP));
		g.drawText(temp, pos, color, fontSize_, kuto::Font::NORMAL);
	}
}
