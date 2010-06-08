/**
 * @file
 * @brief Inventory
 * @author project.kuto
 */

#include "game_inventory.h"


GameInventory::GameInventory(const CRpgLdb& ldb)
: rpgLdb_(ldb), money_(0)
{
	itemList_.resize(rpgLdb_.saItem.GetSize());
	for (uint i = 0; i < itemList_.size(); i++) {
		itemList_[i] = 0;
	}
}

GameInventory::~GameInventory()
{
}

