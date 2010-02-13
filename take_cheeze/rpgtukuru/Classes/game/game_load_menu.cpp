/**
 * @file
 * @brief Load Menu
 * @author project.kuto
 */

#include "game_load_menu.h"
#include "game_system.h"


GameLoadMenu::GameLoadMenu(kuto::Task* parent, GameSystem& gameSystem)
: kuto::Task(parent)
{
	menu_.create(this, gameSystem, false);
}

bool GameLoadMenu::initialize()
{
	if (menu_.initialize()) {
		freeze(true);
		return true;
	}
	return false;
}

void GameLoadMenu::start()
{
	freeze(false);
	menu_.start();
}
