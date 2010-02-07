/**
 * @file
 * @brief Game System Menu
 * @author takuto
 */
#pragma once

#include <kuto/kuto_irender.h>
#include <kuto/kuto_task.h>
#include <kuto/kuto_texture.h>
#include <kuto/kuto_math.h>

class GameField;
class GameSelectWindow;
class GameMessageWindow;
class GameSystemMenuBase;
class GameCharaSelectMenu;


class GameSystemMenu : public kuto::Task, public kuto::IRender
{
public:
	enum State {
		kStateNone,
		kStateTop,
		kStateChara,
		kStateChild,
	};
	enum TopMenu {
		kTopMenuItem,
		kTopMenuSkill,
		kTopMenuEquip,
		kTopMenuSave,
		kTopMenuEndGame,
	};
	
public:
	static GameSystemMenu* createTask(GameField* gameField) { return new GameSystemMenu(gameField); }

private:
	GameSystemMenu(GameField* gameField);
	virtual ~GameSystemMenu();
	
	virtual bool initialize();
	virtual void update();
	virtual void draw();
	
	void updateMoneyWindow();

public:
	void start();
	bool isEnd() const { return state_ == kStateNone; }
	
	virtual void render();
	
private:
	GameField*			gameField_;
	GameSelectWindow*	topMenu_;
	GameCharaSelectMenu*	charaMenu_;
	GameMessageWindow*	moneyWindow_;
	State				state_;
	GameSystemMenuBase*	childMenu_;
};
