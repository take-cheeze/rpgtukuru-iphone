/**
 * @file
 * @brief Inventory
 * @author takuto
 */
#pragma once

#include <kuto/kuto_utility.h>
#include "CRpgLdb.h"


class GameInventory
{
public:
	typedef std::vector<char> ItemList;
	
public:
	GameInventory(const CRpgLdb& ldb);
	~GameInventory();

	int getMoney() const { return money_; }
	void setMoney(int value) { money_ = value; }
	void addMoney(int value) { money_ = kuto::max(0, kuto::min(999999, money_ + value)); }
	int getItemNum(int itemId) const { return itemList_[itemId]; }
	void setItemNum(int itemId, int num) { itemList_[itemId] = num; }
	void addItemNum(int itemId, int num) { itemList_[itemId] = kuto::max(0, kuto::min(99, itemList_[itemId] + num)); }
	const ItemList& getItemList() const { return itemList_; }
	
private:
	const CRpgLdb&		rpgLdb_;
	int					money_;
	ItemList			itemList_;
};

