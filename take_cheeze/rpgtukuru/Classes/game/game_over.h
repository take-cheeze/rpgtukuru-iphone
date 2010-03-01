/**
 * @file
 * @brief Game Over
 * @author project.kuto
 */

#include <kuto/kuto_irender.h>
#include <kuto/kuto_task.h>
#include <kuto/kuto_texture.h>
#include <kuto/kuto_math.h>
#include "game_system.h"

class Game;


class GameOver : public kuto::Task, public kuto::IRender
{
public:
	static GameOver* createTask(Game* parent, GameSystem& gameSystem) { return new GameOver(parent, gameSystem); }

private:
	GameOver(Game* parent, GameSystem& gameSystem);
	
	virtual bool initialize();
	virtual void update();
	virtual void draw();

public:
	virtual void render(kuto::Graphics2D& g);
	// virtual void render(kuto::Graphics& g);
	
private:
	Game*				game_;
	GameSystem&			gameSystem_;
	kuto::Texture		texture_;
};
