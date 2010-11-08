/**
 * @file
 * @brief Game Over
 * @author project.kuto
 */
#pragma once

#include <kuto/kuto_irender.h>
#include <kuto/kuto_texture.h>
#include <kuto/kuto_math.h>

namespace rpg2k { namespace model { class Project; } }

class Game;


class GameOver : public kuto::IRender2D, public kuto::TaskCreatorParam1<GameOver, Game&>
{
	friend class kuto::TaskCreatorParam1<GameOver, Game&>;
private:
	GameOver(Game& parent);

	virtual bool initialize();
	virtual void update();

public:
	virtual void render(kuto::Graphics2D& g) const;

private:
	Game&				game_;
	rpg2k::model::Project&			project_;
	kuto::Texture		texture_;
};
