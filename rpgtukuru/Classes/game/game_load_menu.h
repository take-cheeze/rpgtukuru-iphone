/**
 * @file
 * @brief Load Menu
 * @author takuto
 */
#pragma once

#include <kuto/kuto_task.h>
#include "game_saveload_menu.h"

class GameSystem;


class GameLoadMenu : public kuto::Task
{
public:
	static GameLoadMenu* createTask(kuto::Task* parent, GameSystem& gameSystem) { return new GameLoadMenu(parent, gameSystem); }

private:
	GameLoadMenu(kuto::Task* parent, GameSystem& gameSystem);
	
	virtual bool initialize();

public:
	void start();
	bool selected() const { return menu_.selected(); }
	bool canceled() const { return menu_.canceled(); }
	int selectSaveId() const { return menu_.selectIndex() + 1; }

private:
	GameSaveLoadMenu		menu_;
};
