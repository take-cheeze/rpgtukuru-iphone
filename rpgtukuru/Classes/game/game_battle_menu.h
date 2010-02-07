/**
 * @file
 * @brief Game Battle Menu
 * @author takuto
 */
#pragma once

#include <kuto/kuto_task.h>
#include <kuto/kuto_irender.h>
#include "game_system.h"

class GameSelectWindow;
class GameMessageWindow;
class GameBattle;


class GameBattleMenu : public kuto::Task, public kuto::IRender
{
public:
	enum PageType {
		kPageTop,
		kPageChara,
		kPageCommand,
		kPageSkill,
		kPageItem,
		kPageTarget,
		kPageTargetFriends,
		kPageMax,
	};
	enum PartyCommand {
		kPartyCommandManual,
		kPartyCommandAuto,
		kPartyCommandEscape,
	};
	
public:
	static GameBattleMenu* createTask(GameBattle* battle) { return new GameBattleMenu(battle); }

	virtual void render();
	bool decided() const { return decided_; }
	PartyCommand getPartyCommand() const { return partyCommand_; }
	void reset();
	void updateCharaWindow();
	
private:
	GameBattleMenu(GameBattle* battle);
	
	virtual bool initialize();
	virtual void update();
	virtual void draw();
	
	void setPage(int newPage);
	void changePlayer(int index);
	void nextPlayer(int nextIndex);
	void backPlayer(int backIndex);
	
private:
	GameBattle*			gameBattle_;
	int					page_;
	int					oldPage_;
	GameSelectWindow*	selectWindows_[kPageMax];
	GameMessageWindow*	descriptionWindow_;
	int					animationCounter_;
	std::vector<int>	selectIdList_;
	bool				decided_;
	PartyCommand		partyCommand_;
	int					attackType_;
	int					attackId_;
};
