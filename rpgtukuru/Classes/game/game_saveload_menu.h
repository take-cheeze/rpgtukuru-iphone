/**
 * @file
 * @brief Save Load Menu Base
 * @author project.kuto
 */
#pragma once

#include <kuto/kuto_task.h>

namespace rpg2k { namespace model { class Project; } }

class Game;
class GameMessageWindow;
class GameSelectWindow;


class GameSaveLoadMenu : public kuto::Task
{
public:
	enum State {
		kStateInit,
		kStateNone,
		kStateTop,
		kStateRewrite,
	};

public:
	GameSaveLoadMenu(Game& g, bool modeSave);

	bool initialize();
	bool selected() const;
	bool canceled() const;
	int selectIndex() const;
	void start();
	rpg2k::model::Project& project() { return project_; }

protected:
	void setDiscriptionMessage();

private:
	virtual void update();

private:
	Game& game_;
	rpg2k::model::Project&			project_;
	State				state_;
	GameSelectWindow&	topMenu_;
	GameMessageWindow&	descriptionWindow_;
	bool				modeSave_;
};
