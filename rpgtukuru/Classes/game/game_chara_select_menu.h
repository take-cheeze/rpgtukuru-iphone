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


class GameCharaSelectMenu : public GameSelectWindow, public kuto::TaskCreatorParam1<GameCharaSelectMenu, GameField&>
{
	friend class kuto::TaskCreatorParam1<GameCharaSelectMenu, GameField&>;
private:
	GameCharaSelectMenu(GameField& gameField);
	virtual void update();
	void renderPlayerInfo(kuto::Graphics2D& g, int index) const;

public:
	virtual void render(kuto::Graphics2D& g) const;
	void useFullSelectKey(bool value = true) { useFullSelectKey_ = value; }
	bool isUseFullSelectKey() const { return useFullSelectKey_; }

private:
	GameField&		field_;
	bool			useFullSelectKey_;
};
