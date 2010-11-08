/**
 * @file
 * @brief Load Menu
 * @author project.kuto
 */

#include "game_load_menu.h"
#include <rpg2k/Project.hpp>


GameLoadMenu::GameLoadMenu(Game& g)
: GameSaveLoadMenu(g, false)
{
}

bool GameLoadMenu::initialize()
{
	if (GameSaveLoadMenu::initialize()) {
		freeze();
		return true;
	}
	return false;
}

void GameLoadMenu::start()
{
	freeze(false);
	GameSaveLoadMenu::start();
}
