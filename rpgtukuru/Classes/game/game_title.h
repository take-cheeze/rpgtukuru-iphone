/**
 * @file
 * @brief Game Title
 * @author project.kuto
 */

#include <kuto/kuto_irender.h>
#include <kuto/kuto_texture.h>
#include <kuto/kuto_math.h>
#include "game_system.h"

class GameSelectWindow;
class GameLoadMenu;


class GameTitle : public kuto::IRender2D, public kuto::TaskCreatorParam1<GameTitle, rpg2k::model::Project&>
{
	friend class kuto::TaskCreatorParam1<GameTitle, rpg2k::model::Project&>;
public:
	enum SelectMenuType {
		kSelectNewGame,
		kSelectContinue,
		kSelectShutDown,
		kSelectNone,
	};

private:
	GameTitle(rpg2k::model::Project& gameSystem);

	virtual bool initialize();
	virtual void update();

public:
	virtual void render(kuto::Graphics2D* g) const;
	SelectMenuType getSelectMenu() const { return selectMenu_; }
	int getSaveId() const;

private:
	rpg2k::model::Project&			gameSystem_;
	kuto::Vector2		screenOffset_;
	kuto::Vector2		screenScale_;
	kuto::Texture		titleTex_;
	GameSelectWindow*	selectWindow_;
	GameLoadMenu*		loadMenu_;
	SelectMenuType		selectMenu_;
};
