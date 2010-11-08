/**
 * @file
 * @brief Save Menu
 * @author project.kuto
 */
#pragma once

#include <kuto/kuto_irender.h>
#include "game_system_menu_base.h"
#include "game_saveload_menu.h"

class GameField;


class GameSaveMenu : public GameSystemMenuBase, public kuto::TaskCreatorParam1<GameSaveMenu, GameField&>
{
	friend class kuto::TaskCreatorParam1<GameSaveMenu, GameField&>;
public:
	enum State {
		kStateTop = kStateCustom,
		kStateRewrite,
	};

private:
	GameSaveMenu(GameField& gameField);

	virtual bool initialize();
	virtual void update();

public:
	virtual void start();
	virtual void render(kuto::Graphics2D& g) const;

private:
	GameSaveLoadMenu*		menu_;
};
