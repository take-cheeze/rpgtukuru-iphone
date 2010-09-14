/**
 * @file
 * @brief Save Load Menu Base
 * @author project.kuto
 */
#pragma once

#include <kuto/kuto_task.h>

namespace rpg2k { namespace model { class Project; } }
class GameSelectWindow;
class GameMessageWindow;


class GameSaveLoadMenu : public kuto::Task, public kuto::TaskCreatorParam2<GameSaveLoadMenu, rpg2k::model::Project&, bool>
{
	friend class kuto::TaskCreatorParam2<GameSaveLoadMenu, rpg2k::model::Project&, bool>;
public:
	enum State {
		kStateInit,
		kStateNone,
		kStateTop,
		kStateRewrite,
	};
	// enum { SAVE_MAX = 15, };

public:
	GameSaveLoadMenu(rpg2k::model::Project& gameSystem, bool modeSave);

	bool initialize();
	bool selected() const;
	bool canceled() const;
	int selectIndex() const;
	void start();
	rpg2k::model::Project* getGameSystem() { return gameSystem_; }

protected:
	void setDiscriptionMessage();
	// void readHeaders();

private:
	virtual void update();

private:
	rpg2k::model::Project*			gameSystem_;
	State				state_;
	GameSelectWindow*	topMenu_;
	GameMessageWindow*	descriptionWindow_;
	bool				modeSave_;
};
