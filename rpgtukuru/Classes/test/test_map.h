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
#include "CRpgLdb.h"
#include "CRpgLmu.h"
#include "CRpgLmt.h"


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
	CRpgLdb				rpgLdb_;
	CRpgLmu				rpgLmu_;
	CRpgLmt				rpgLmt_;
	int					animationCounter_;
	kuto::Vector2		screenOffset_;
	kuto::Vector2		screenScale_;
};

