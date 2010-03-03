/**
 * @file
 * @brief GameNameInputMenu
 * @author project.kuto
 */
#pragma once

#include <kuto/kuto_task.h>
#include <kuto/kuto_math.h>

class GameNameSelectWindow;
class GameMessageWindow;
class GameSystem;


class GameNameInputMenu : public kuto::Task
{
public:
	static GameNameInputMenu* createTask(kuto::Task* parent, GameSystem& gameSystem) { return new GameNameInputMenu(parent, gameSystem); }

	void setPlayerInfo(int playerId, bool katakana, bool useDefaultName);
	bool closed() const { return closed_; }
	std::string getName() const { return name_; }
	
private:
	GameNameInputMenu(kuto::Task* parent, GameSystem& gameSystem);
	virtual ~GameNameInputMenu();
	
	virtual bool initialize();
	virtual void update();
	
private:
	GameSystem&				gameSystem_;
	int						playerId_;
	GameNameSelectWindow*	selectWindow_;
	GameMessageWindow*		charaWindow_;
	GameMessageWindow*		nameWindow_;
	int						inputIndex_;
	bool					closed_;
	std::string				name_;
};
