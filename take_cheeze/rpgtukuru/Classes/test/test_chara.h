/**
 * @file
 * @brief Chara Test
 * @author project.kuto
 */

#include <kuto/kuto_irender.h>
#include <kuto/kuto_task.h>
#include <kuto/kuto_math.h>
#include <rpg2kLib/Project.hpp>

class GameChara;
class GameMap;

class TestChara : public kuto::Task, public kuto::IRender
{
public:
	static TestChara* createTask(kuto::Task* parent) { return new TestChara(parent); }

private:
	TestChara(kuto::Task* parent);
	
	virtual bool initialize();
	virtual void update();
	virtual void draw();

public:
	virtual void render();
	
private:
	rpg2kLib::model::DataBase				rpgLdb_;
	int					animationCounter_;
	kuto::Vector2		screenOffset_;
	kuto::Vector2		screenScale_;
	GameChara*			gameChara_;
	GameMap*			gameMap_;
	bool				drawFace_;
};
