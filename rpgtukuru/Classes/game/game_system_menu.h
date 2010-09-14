/**
 * @file
 * @brief Game System Menu
 * @author project.kuto
 */
#pragma once

#include <kuto/kuto_irender.h>
#include <kuto/kuto_texture.h>
#include <kuto/kuto_math.h>

class GameField;
class GameSelectWindow;
class GameMessageWindow;
class GameSystemMenuBase;
class GameCharaSelectMenu;


class GameSystemMenu : public kuto::IRender2D, public kuto::TaskCreatorParam1<GameSystemMenu, GameField*>
{
	friend class kuto::TaskCreatorParam1<GameSystemMenu, GameField*>;
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

private:
	GameSystemMenu(GameField* gameField);

	virtual bool initialize();
	virtual void update();

	void updateMoneyWindow();

public:
	void start();
	bool isEnd() const { return state_ == kStateNone; }

	virtual void render(kuto::Graphics2D* g) const;

private:
	GameField*			gameField_;
	GameSelectWindow*	topMenu_;
	GameCharaSelectMenu*	charaMenu_;
	GameMessageWindow*	moneyWindow_;
	State				state_;
	GameSystemMenuBase*	childMenu_;
};
