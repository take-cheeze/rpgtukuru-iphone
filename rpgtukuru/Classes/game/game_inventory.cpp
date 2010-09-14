/**
 * @file
 * @brief Inventory
 * @author project.kuto
 */

#include "game_inventory.h"


GameInventory::GameInventory(const rpg2k::model::DataBase& ldb)
: rpgLdb_(ldb), money_(0)
{
	itemList_.resize(rpgLdb_.item().rend().first() + 1, 0);
}
