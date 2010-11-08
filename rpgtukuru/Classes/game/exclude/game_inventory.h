/**
 * @file
 * @brief Inventory
 * @author project.kuto
 */
#pragma once

#include <kuto/kuto_utility.h>
#include <rpg2k/DataBase.hpp>


class GameInventory
{
public:
	typedef std::vector< kuto::u8 > ItemList;
public:
	GameInventory(const rpg2k::model::DataBase& ldb);
	~GameInventory();

	int getMoney() const { return money_; }
	void setMoney(int value) { money_ = value; }
	void addMoney(int value) { money_ = kuto::max(0, kuto::min(999999, money_ + value)); }
	int getItemNum(int itemId) const { return itemList_[itemId]; }
	void setItemNum(int itemId, int num) { itemList_[itemId] = num; }
	void addItemNum(int itemId, int num) { itemList_[itemId] = kuto::max(0, kuto::min(99, itemList_[itemId] + num)); }
	const ItemList& getItemList() const { return itemList_; }
private:
	const rpg2k::model::DataBase&		rpgLdb_;
	int					money_;
	ItemList			itemList_;
};

