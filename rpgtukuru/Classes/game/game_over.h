/**
 * @file
 * @brief Game Over
 * @author project.kuto
 */

#include <kuto/kuto_irender.h>
#include <kuto/kuto_texture.h>
#include <kuto/kuto_math.h>
#include "game_system.h"

class Game;


class GameOver : public kuto::IRender2D, public kuto::TaskCreatorParam2<GameOver, Game*, rpg2k::model::Project&>
{
	friend class kuto::TaskCreatorParam2<GameOver, Game*, rpg2k::model::Project&>;
private:
	GameOver(Game* parent, rpg2k::model::Project& gameSystem);

	virtual bool initialize();
	virtual void update();

public:
	virtual void render(kuto::Graphics2D* g) const;

private:
	Game*				game_;
	rpg2k::model::Project&			gameSystem_;
	kuto::Texture		texture_;
};
