/**
 * @file
 * @brief Load Menu
 * @author project.kuto
 */
#pragma once

#include <kuto/kuto_task.h>
#include "game_saveload_menu.h"

namespace rpg2k { namespace model { class Project; } }


class GameLoadMenu : public kuto::Task
{
public:
	static GameLoadMenu* createTask(kuto::Task* parent, rpg2k::model::Project& gameSystem) { return new GameLoadMenu(parent, gameSystem); }

private:
	GameLoadMenu(kuto::Task* parent, rpg2k::model::Project& gameSystem);

	virtual bool initialize();

public:
	void start();
	bool selected() const { return menu_.selected(); }
	bool canceled() const { return menu_.canceled(); }
	int selectSaveId() const { return menu_.selectIndex() + 1; }

private:
	GameSaveLoadMenu		menu_;
};
