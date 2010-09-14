/**
 * @file
 * @brief Name Select
 * @author project.kuto
 */
#pragma once

#include "game_select_window.h"


class GameNameSelectWindow : public GameSelectWindow, public kuto::TaskCreatorParam1<GameNameSelectWindow, const rpg2k::model::Project&>
{
	friend class kuto::TaskCreatorParam1<GameNameSelectWindow, const rpg2k::model::Project&>;
public:
	enum KanaType {
		kHiragana,
		kKatakana,
		kKanaMax
	};
	enum SpecialButton {
		kKanaButton = 86,
		kApplyButton = 88,
	};

private:
	GameNameSelectWindow(const rpg2k::model::Project& gameSystem);
	virtual void update();

	void renderSelectCursor(kuto::Graphics2D* g) const;

public:
	virtual void render(kuto::Graphics2D* g) const;
	void setKana(KanaType type);

private:
	KanaType		kanaType_;
};

