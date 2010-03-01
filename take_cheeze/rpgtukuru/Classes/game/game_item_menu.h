/**
 * @file
 * @brief Item Menu
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
class GameCharaSelectMenu;


class GameItemMenu : public GameSystemMenuBase, public kuto::IRender
{
public:
	enum State {
		kStateItem = kStateCustom,
		kStateChara,
	};
	
public:
	static GameItemMenu* createTask(GameField* gameField) { return new GameItemMenu(gameField); }

private:
	GameItemMenu(GameField* gameField);
	virtual ~GameItemMenu();
	
	virtual bool initialize();
	virtual void update();
	virtual void draw();
	
	void updateDiscriptionMessage();
	void updateItemWindow();
	void setState(int newState);
	bool applyItem(int itemId, int playerId);

public:
	virtual void start();
	virtual void render(kuto::Graphics2D& g);
	// virtual void render(kuto::Graphics& g);
	
private:
	GameSelectWindow*	itemMenu_;
	GameMessageWindow*	descriptionWindow_;
	GameCharaSelectMenu*	charaMenu_;
	GameMessageWindow*		itemNameWindow_;
	GameMessageWindow*		itemNumWindow_;
	std::vector<int>	itemList_;
};
