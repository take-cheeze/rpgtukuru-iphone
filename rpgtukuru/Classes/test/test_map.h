/**
 * @file
 * @brief Map Draw Test
 * @author project.kuto
 */
#pragma once

#include <kuto/kuto_irender.h>
#include <kuto/kuto_task.h>
#include <kuto/kuto_texture.h>
#include <kuto/kuto_math.h>
#include <rpg2k/DataBase.hpp>
#include <rpg2k/MapUnit.hpp>
#include <rpg2k/MapTree.hpp>


class TestMap : public kuto::Task, public kuto::IRender
{
public:
	static TestMap* createTask(kuto::Task* parent) { return new TestMap(parent); }

private:
	TestMap(kuto::Task* parent);

	virtual bool initialize();
	virtual void update();
	virtual void draw();

public:
	virtual void render();

private:
	rpg2k::model::DataBase				rpgLdb_;
	rpg2k::model::MapUnit				rpgLmu_;
	rpg2k::model::MapTree				rpgLmt_;
	int					animationCounter_;
	kuto::Vector2		screenOffset_;
	kuto::Vector2		screenScale_;
};

