/**
 * @file
 * @brief Inventory
 * @author project.kuto
 */

#include "game_inventory.h"

using namespace rpg2kLib::model;


GameInventory::GameInventory(const DataBase& ldb)
: rpgLdb_(ldb), money_(0)
{
	itemList_.resize( ( --rpgLdb_.getItem().end() ).first()+1 );
	for (u32 i = 0; i < itemList_.size(); i++) {
		itemList_[i] = 0;
	}
}

GameInventory::~GameInventory()
{
}

