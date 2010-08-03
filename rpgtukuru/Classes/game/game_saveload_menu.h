/**
 * @file
 * @brief Save Load Menu Base
 * @author project.kuto
 */
#pragma once

#include <kuto/kuto_task.h>
#include "game_save_data.h"

namespace rpg2k { namespace model { class Project; } }
class GameSelectWindow;
class GameMessageWindow;


class GameSaveLoadMenu
{
public:
	enum State {
		kStateInit,
		kStateNone,
		kStateTop,
		kStateRewrite,
	};
	enum {
		SAVE_MAX = 15,
	};

public:
	GameSaveLoadMenu();

	void create(kuto::Task* parent, rpg2k::model::Project& gameSystem, bool modeSave);
	bool initialize();
	bool selected() const;
	bool canceled() const;
	int selectIndex() const;
	void start();
	rpg2k::model::Project* getGameSystem() { return gameSystem_; }

protected:
	void setDiscriptionMessage();
	void readHeaders();

private:
	rpg2k::model::Project*			gameSystem_;
	State				state_;
	GameSelectWindow*	topMenu_;
	GameMessageWindow*	descriptionWindow_;
	GameSaveDataHeader	headers_[SAVE_MAX];
	bool				enableHeaders_[SAVE_MAX];
	bool				modeSave_;
};
