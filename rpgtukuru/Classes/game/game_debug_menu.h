/**
 * @file
 * @brief Debug Menu
 * @author project.kuto
 */
#pragma once

#include <kuto/kuto_task.h>

class GameField;
class GameSelectWindow;
class GameMessageWindow;
class GameCharaSelectMenu;


class GameDebugMenu : public kuto::Task, public kuto::TaskCreatorParam1<GameDebugMenu, GameField*>
{
	friend class kuto::TaskCreatorParam1<GameDebugMenu, GameField*>;
public:
	enum State {
		kStateNone,
		kStateTop,
		kStateChara,
	};
	enum DebugType {
		kDebugCure,
		kDebugKill,
		kDebugLevelUp,
		kDebugLevelDown,
		kDebugMoneyUp,
		kDebugMoneyDown,
		kDebugItemUp,
		kDebugItemDown,
		kDebugNoEncount,
		kDebugAlwaysEscape,
		kDebugPlayerDash,
		kDebugThroughCollision,
		kDebugDifficulty,
		kDebugMax
	};

private:
	GameDebugMenu(GameField* gameField);

	virtual bool initialize();
	virtual void update();

	void updateDiscriptionMessage();
	void setState(State newState);
	void applyDebug(int debugId, int playerId);
	void updateTopMenu();

private:
	GameField*				gameField_;
	State					state_;
	GameSelectWindow*		topMenu_;
	GameMessageWindow*		descriptionWindow_;
	GameCharaSelectMenu*	charaMenu_;
	GameMessageWindow*		debugNameWindow_;
};
