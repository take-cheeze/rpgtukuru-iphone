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


class GameEquipMenu : public GameSystemMenuBase
{
public:
	enum State {
		kStateEquip = kStateCustom,
		kStateItem,
	};

public:
	static GameEquipMenu* createTask(GameField* gameField, GameCharaStatus* charaStatus) { return new GameEquipMenu(gameField, charaStatus); }

private:
	GameEquipMenu(GameField* gameField, GameCharaStatus* charaStatus);

	virtual bool initialize();
	virtual void update();

	void setDiscriptionMessage();
	void updateItemWindow();
	void setState(int newState);

public:
	virtual void start();
	virtual void render(kuto::Graphics2D* g) const;

private:
	GameSelectWindow*	equipMenu_;
	GameSelectWindow*	itemMenu_;
	GameMessageWindow*	descriptionWindow_;
	GameMessageWindow*	statusWindow_;
	GameCharaStatus*	charaStatus_;
	std::vector<int>	itemList_;
};
