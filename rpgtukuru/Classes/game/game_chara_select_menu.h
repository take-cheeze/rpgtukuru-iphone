/**
 * @file
 * @brief Select Chara Window
 * @author project.kuto
 */
#pragma once

#include <kuto/kuto_texture.h>
#include <kuto/kuto_math.h>
#include "game_select_window.h"

class GameField;
class GamePlayer;


class GameCharaSelectMenu : public GameSelectWindow
{
public:
	static GameCharaSelectMenu* createTask(kuto::Task* parent, GameField* gameField) { return new GameCharaSelectMenu(parent, gameField); }

private:
	GameCharaSelectMenu(kuto::Task* parent, GameField* gameField);
	virtual void update();
	void renderPlayerInfo(int index);
	
public:
	virtual void render();
	void setUseFullSelectKey(bool value) { useFullSelectKey_ = value; }
	bool isUseFullSelectKey() const { return useFullSelectKey_; }

private:
	GameField*		gameField_;
	bool			useFullSelectKey_;
};
