/**
 * @file
 * @brief Title Test
 * @author takuto
 */

#include <kuto/kuto_irender.h>
#include <kuto/kuto_task.h>
#include <kuto/kuto_texture.h>
#include <kuto/kuto_math.h>
#include "CRpgLdb.h"


class TestTitle : public kuto::Task, public kuto::IRender
{
public:
	static TestTitle* createTask(kuto::Task* parent) { return new TestTitle(parent); }

private:
	TestTitle(kuto::Task* parent);
	
	virtual bool initialize();
	virtual void update();
	virtual void draw();

public:
	virtual void render();
	
private:
	CRpgLdb				rpgLdb_;
	int					animationCounter_;
	kuto::Vector2		screenOffset_;
	kuto::Vector2		screenScale_;
	kuto::Texture		titleTex_;
	kuto::Texture		gameoverTex_;
	kuto::Texture		systemTex_;
	bool				drawTitle_;
	int					cursor_;
};
