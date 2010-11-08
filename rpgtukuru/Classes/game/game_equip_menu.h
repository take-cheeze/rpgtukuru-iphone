/**
 * @file
 * @brief Equip Menu
 * @author project.kuto
 */
#pragma once

#include <kuto/kuto_irender.h>
#include <kuto/kuto_texture.h>
#include <kuto/kuto_math.h>
#include "game_system_menu_base.h"

class GameField;
class GameSelectWindow;
class GameMessageWindow;
class GameCharaStatus;


class GameEquipMenu
: public GameSystemMenuBase, public kuto::TaskCreatorParam2<GameEquipMenu, GameField&, int>
{
	friend class kuto::TaskCreatorParam2<GameEquipMenu, GameField&, int>;
public:
	enum State {
		kStateEquip = kStateCustom,
		kStateItem,
	};

private:
	GameEquipMenu(GameField& gameField, int charaID);

	virtual bool initialize();
	virtual void update();

	void setDiscriptionMessage();
	void updateItemWindow();
	void setState(int newState);

	virtual void render(kuto::Graphics2D& g) const;

public:
	virtual void start();

private:
	GameSelectWindow&	equipMenu_;
	GameSelectWindow&	itemMenu_;
	GameMessageWindow&	descriptionWindow_;
	GameMessageWindow&	statusWindow_;
	int const charaID_;
	std::vector<int>	itemList_;
};
