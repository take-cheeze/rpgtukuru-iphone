/**
 * @file
 * @brief Font Draw Test
 * @author project.kuto
 */
#pragma once

#include <kuto/kuto_irender.h>
#include <kuto/kuto_task.h>
#include <kuto/kuto_texture.h>
#include <rpg2kLib/Project.hpp>
#include <rpg2kLib/Project.hpp>


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
	virtual void render(kuto::Graphics2D& g);
	// virtual void render(kuto::Graphics& g);

private:
	kuto::Texture		texture_;
};

