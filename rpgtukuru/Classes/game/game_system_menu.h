/**
 * @file
 * @brief Game System Menu
 * @author project.kuto
 */
#pragma once

#include <kuto/kuto_irender.h>
#include <kuto/kuto_math.h>
#include <kuto/kuto_texture.h>

class GameCharaSelectMenu;
class GameField;
class GameMessageWindow;
class GameSelectWindow;
class GameSystemMenuBase;


class GameSystemMenu : public kuto::IRender2D, public kuto::TaskCreatorParam1<GameSystemMenu, GameField&>
{
	friend class kuto::TaskCreatorParam1<GameSystemMenu, GameField&>;
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
	GameSystemMenu(GameField& gameField);

	virtual bool initialize();
	virtual void update();

	void updateMoneyWindow();

	virtual void render(kuto::Graphics2D& g) const;

public:
	void start();
	bool isEnd() const { return state_ == kStateNone; }

private:
	GameField&			field_;
	GameSelectWindow*	topMenu_;
	GameCharaSelectMenu*	charaMenu_;
	GameMessageWindow*	moneyWindow_;
	State				state_;
	GameSystemMenuBase*	childMenu_;
};
