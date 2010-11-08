/**
 * @file
 * @brief Game Title
 * @author project.kuto
 */
#pragma once

#include <kuto/kuto_irender.h>
#include <kuto/kuto_texture.h>
#include <kuto/kuto_math.h>

namespace rpg2k { namespace model { class Project; } }

class Game;
class GameLoadMenu;
class GameSelectWindow;


class GameTitle : public kuto::IRender2D, public kuto::TaskCreatorParam1<GameTitle, Game&>
{
	friend class kuto::TaskCreatorParam1<GameTitle, Game&>;
public:
	enum SelectMenuType {
		kSelectNewGame,
		kSelectContinue,
		kSelectShutDown,
		kSelectNone,
	};

private:
	GameTitle(Game& g);

	virtual bool initialize();
	virtual void update();

public:
	virtual void render(kuto::Graphics2D& g) const;
	SelectMenuType selectMenu() const { return selectMenu_; }
	int saveID() const;

private:
	Game& game_;
	rpg2k::model::Project&			project_;
	kuto::Vector2		screenOffset_;
	kuto::Vector2		screenScale_;
	kuto::Texture		titleTex_;
	GameSelectWindow&	selectWindow_;
	GameLoadMenu*		loadMenu_;
	SelectMenuType		selectMenu_;
};
