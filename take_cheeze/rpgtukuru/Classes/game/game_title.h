/**
 * @file
 * @brief Game Title
 * @author project.kuto
 */

#include <kuto/kuto_irender.h>
#include <kuto/kuto_task.h>
#include <kuto/kuto_texture.h>
#include <kuto/kuto_math.h>
#include "game_system.h"

class GameSelectWindow;
class GameLoadMenu;


class GameTitle : public kuto::Task, public kuto::IRender
{
public:
	enum SelectMenuType {
		kSelectNewGame,
		kSelectContinue,
		kSelectShutDown,
		kSelectNone,
	};
	
	static GameTitle* createTask(kuto::Task* parent, GameSystem& gameSystem) { return new GameTitle(parent, gameSystem); }

private:
	GameTitle(kuto::Task* parent, GameSystem& gameSystem);
	
	virtual bool initialize();
	virtual void update();
	virtual void draw();

public:
	virtual void render(kuto::Graphics2D& g);
	// virtual void render(kuto::Graphics& g);
	SelectMenuType getSelectMenu() const { return selectMenu_; }
	int getSaveId() const;
	
private:
	GameSystem&			gameSystem_;
	kuto::Vector2		screenOffset_;
	kuto::Vector2		screenScale_;
	kuto::Texture		titleTex_;
	GameSelectWindow*	selectWindow_;
	GameLoadMenu*		loadMenu_;
	SelectMenuType		selectMenu_;
};
