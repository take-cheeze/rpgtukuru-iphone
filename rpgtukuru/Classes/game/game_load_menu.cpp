/**
 * @file
 * @brief Load Menu
 * @author project.kuto
 */

#include "game_load_menu.h"
#include "game_system.h"


GameLoadMenu::GameLoadMenu(rpg2k::model::Project& gameSystem)
: GameSaveLoadMenu(gameSystem, false)
{
}

bool GameLoadMenu::initialize()
{
	if (GameSaveLoadMenu::initialize()) {
		freeze(true);
		return true;
	}
	return false;
}

void GameLoadMenu::start()
{
	freeze(false);
	GameSaveLoadMenu::start();
}
