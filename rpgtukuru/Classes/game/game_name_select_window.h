/**
 * @file
 * @brief Name Select
 * @author project.kuto
 */
#pragma once

#include "game_select_window.h"


class GameNameSelectWindow : public GameSelectWindow
{
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
	
public:
	static GameNameSelectWindow* createTask(kuto::Task* parent, const GameSystem& gameSystem) { return new GameNameSelectWindow(parent, gameSystem); }

private:
	GameNameSelectWindow(kuto::Task* parent, const GameSystem& gameSystem);
	virtual void update();

	void renderSelectCursor();
	
public:
	virtual void render();
	void setKana(KanaType type);
	
private:
	KanaType		kanaType_;
};

