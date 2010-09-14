/**
 * @file
 * @brief Skill Menu
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


class GameSkillMenu : public GameSystemMenuBase
{
public:
	enum State {
		kStateSkill = kStateCustom,
		kStateChara,
	};

public:
	static GameSkillMenu* createTask(GameField* gameField, GameCharaStatus* charaStatus) { return new GameSkillMenu(gameField, charaStatus); }

private:
	GameSkillMenu(GameField* gameField, GameCharaStatus* charaStatus);

	virtual bool initialize();
	virtual void update();

	void updateDiscriptionMessage();
	void updateSkillWindow();
	void setState(int newState);
	bool applySkill(int skillId, int playerId);

public:
	virtual void start();
	virtual void render(kuto::Graphics2D* g) const;

private:
	GameSelectWindow*	skillMenu_;
	GameMessageWindow*	descriptionWindow_;
	GameMessageWindow*	charaStatusWindow_;
	GameCharaSelectMenu*	charaMenu_;
	GameMessageWindow*		skillNameWindow_;
	GameMessageWindow*		mpWindow_;
	GameCharaStatus*	charaStatus_;
	std::vector<int>	skillList_;
};
