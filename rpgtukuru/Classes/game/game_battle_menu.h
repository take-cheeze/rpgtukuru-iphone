/**
 * @file
 * @brief Game Battle Menu
 * @author project.kuto
 */
#pragma once

#include <kuto/kuto_irender.h>
#include <vector>

class GameBattle;
class GameMessageWindow;
class GameSelectWindow;

namespace rpg2k { namespace model { class Project; } }


class GameBattleMenu : public kuto::IRender2D, public kuto::TaskCreatorParam1<GameBattleMenu, GameBattle&>
{
	friend class kuto::TaskCreatorParam1<GameBattleMenu, GameBattle&>;
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
	virtual void render(kuto::Graphics2D& g) const;
	bool decided() const { return decided_; }
	PartyCommand partyCommand() const { return partyCommand_; }
	void reset();
	void updateCharaWindow();

private:
	GameBattleMenu(GameBattle& battle);

	virtual bool initialize();
	virtual void update();

	void setPage(int newPage);
	void changePlayer(int index);
	void nextPlayer(int nextIndex);
	void backPlayer(int backIndex);

private:
	GameBattle&			battle_;
	int					page_;
	int					oldPage_;
	GameSelectWindow*	selectWindows_[kPageMax];
	GameMessageWindow&	descriptionWindow_;
	int					animationCounter_;
	std::vector<int>	selectIdList_;
	bool				decided_;
	PartyCommand		partyCommand_;
	int					attackType_;
	int					attackId_;
};
