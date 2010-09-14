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
namespace rpg2k { namespace model { class Project; } }


class GameNameInputMenu : public kuto::Task, public kuto::TaskCreatorParam1<GameNameInputMenu, rpg2k::model::Project&>
{
	friend class kuto::TaskCreatorParam1<GameNameInputMenu, rpg2k::model::Project&>;
public:
	void setPlayerInfo(int playerId, bool katakana, bool useDefaultName);
	bool closed() const { return closed_; }
	std::string getName() const { return name_; }

private:
	GameNameInputMenu(rpg2k::model::Project& gameSystem);

	virtual bool initialize();
	virtual void update();

private:
	rpg2k::model::Project&				gameSystem_;
	int						playerId_;
	GameNameSelectWindow*	selectWindow_;
	GameMessageWindow*		charaWindow_;
	GameMessageWindow*		nameWindow_;
	int						inputIndex_;
	bool					closed_;
	std::string				name_;
};
