/**
 * @file
 * @brief Font Draw Test
 * @author project.kuto
 */
#pragma once

#include <kuto/kuto_irender.h>
#include <kuto/kuto_task.h>
#include <kuto/kuto_texture.h>
#include "CRpgLdb.h"
#include "CRpgLmu.h"


class TestFont : public kuto::Task, public kuto::IRender
{
public:
	static TestFont* createTask(kuto::Task* parent) { return new TestFont(parent); }

private:
	TestFont(kuto::Task* parent);
	
	virtual bool initialize();
	virtual void update();
	virtual void draw();

public:
	virtual void render();

private:
	kuto::Texture		texture_;
};

