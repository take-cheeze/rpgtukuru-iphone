/**
 * @file
 * @brief Chara Test
 * @author project.kuto
 */

#include <kuto/kuto_irender.h>
#include <kuto/kuto_math.h>
#include <rpg2k/DataBase.hpp>

#include "game_chara_status.h"
#include "game_field.h"

// class GameChara;
class GameMap;


class TestChara : public kuto::IRender2D, public kuto::TaskCreator<TestChara>
{
	friend class kuto::TaskCreator<TestChara>;
private:
	TestChara();

	virtual bool initialize();
	virtual void update();

public:
	virtual void render(kuto::Graphics2D& g) const;

private:
	rpg2k::model::DataBase				rpgLdb_;
	int					animationCounter_;
	kuto::Vector2		screenOffset_;
	kuto::Vector2		screenScale_;
	// GameChara*			gameChara_;
	GameMap*			gameMap_;
	bool				drawFace_;
	GameCharaStatus		charaStatus_;
};
