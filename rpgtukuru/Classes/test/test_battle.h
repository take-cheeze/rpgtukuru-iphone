/**
 * @file
 * @brief Battle Test
 * @author project.kuto
 */
#pragma once

#include <kuto/kuto_irender.h>
#include <kuto/kuto_task.h>
#include <kuto/kuto_texture.h>
#include <kuto/kuto_math.h>
#include <kuto/kuto_simple_array.h>
#include "game_system.h"

class GameBattle;


class TestBattle : public kuto::Task, public kuto::IRender
{
public:
	static TestBattle* createTask(kuto::Task* parent) { return new TestBattle(parent); }

private:
	TestBattle(kuto::Task* parent);

	virtual bool initialize();
	virtual void update();
	virtual void draw();

public:
	virtual void render();

private:
	rpg2k::model::Project			gameSystem_;
	GameBattle*			gameBattle_;
};

