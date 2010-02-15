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
#include <rpg2kLib/Project.hpp>

using namespace rpg2kLib::model;

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
	DataBase				rpgLdb_;
	MapTree				rpgLmt_;
	MapUnit				rpgLmu_;
	int					animationCounter_;
	kuto::Vector2		screenOffset_;
	kuto::Vector2		screenScale_;
};

