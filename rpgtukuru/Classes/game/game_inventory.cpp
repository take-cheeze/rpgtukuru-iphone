/**
 * @file
 * @brief Inventory
 * @author takuto
 */

#include "game_inventory.h"


GameInventory::GameInventory(const CRpgLdb& ldb)
: rpgLdb_(ldb), money_(0)
{
	itemList_.resize(rpgLdb_.saItem.GetSize());
	for (u32 i = 0; i < itemList_.size(); i++) {
		itemList_[i] = 0;
	}
}

GameInventory::~GameInventory()
{
}

