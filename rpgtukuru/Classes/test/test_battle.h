/**
 * @file
 * @brief Battle Test
 * @author project.kuto
 */
#pragma once

#include <kuto/kuto_irender.h>
#include <kuto/kuto_texture.h>
#include <kuto/kuto_math.h>
#include <kuto/kuto_simple_array.h>

#include <rpg2k/Project.hpp>

class GameBattle;


class TestBattle : public kuto::IRender2D, public kuto::TaskCreator<TestBattle>
{
	friend class kuto::TaskCreator<TestBattle>;
private:
	TestBattle();

	virtual bool initialize();
	virtual void update();

public:
	virtual void render(kuto::Graphics2D& g) const;

private:
	rpg2k::model::Project			gameSystem_;
	GameBattle*			gameBattle_;
};
