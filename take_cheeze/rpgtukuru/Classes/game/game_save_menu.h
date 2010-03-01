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


class GameSaveMenu : public GameSystemMenuBase, public kuto::IRender
{
public:
	enum State {
		kStateTop = kStateCustom,
		kStateRewrite,
	};
	
public:
	static GameSaveMenu* createTask(GameField* gameField) { return new GameSaveMenu(gameField); }

private:
	GameSaveMenu(GameField* gameField);
	virtual ~GameSaveMenu();
	
	virtual bool initialize();
	virtual void update();
	virtual void draw();

public:
	virtual void start();
	virtual void render(kuto::Graphics2D& g);
	// virtual void render(kuto::Graphics& g);
	
private:
	GameSaveLoadMenu		menu_;
};
