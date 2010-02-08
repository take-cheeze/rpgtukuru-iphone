/**
 * @file
 * @brief Font Draw Test
 * @author project.kuto
 */

#include "test_font.h"
#include <kuto/kuto_render_manager.h>
#include <kuto/kuto_graphics2d.h>
#include <kuto/kuto_touch_pad.h>
#include <kuto/kuto_error.h>
#include <kuto/kuto_utility.h>
#include <kuto/kuto_virtual_pad.h>


TestFont::TestFont(kuto::Task* parent)
: kuto::Task(parent)
{
	kuto::VirtualPad::instance()->pauseDraw(true);
	texture_.loadFromFile("/User/Media/Photos/RPG2000/RTP/Title/タイトル1.png");
}

bool TestFont::initialize()
{
	return isInitializedChildren();
}

void TestFont::update()
{
	if (kuto::TouchPad::instance()->doubleClick(0)) {
		// return debug menu
		this->release();
	}
}

void TestFont::draw()
{
	kuto::RenderManager::instance()->addRender(this, kuto::LAYER_2D_OBJECT, 0.f);
}

void TestFont::render()
{
	kuto::Graphics2D* g = kuto::RenderManager::instance()->getGraphics2D();
	
	g->drawTexture(texture_, kuto::Vector2(0.f, 0.f), kuto::Vector2(320.f, 240.f),
		kuto::Color(1.f, 1.f, 1.f, 1.f), true);
	g->drawText("jypqあお劇厳しい", kuto::Vector2(0.f, 100.f), kuto::Color(1.f, 1.f, 1.f, 1.f),
		28.f, kuto::Font::TYPE_NORMAL);
	g->drawText("Double Tap here to return.", kuto::Vector2(0.f, 150.f), kuto::Color(1.f, 1.f, 0.f, 1.f),
				16.f, kuto::Font::TYPE_NORMAL);
	g->drawText("Good luck!", kuto::Vector2(0.f, 170.f), kuto::Color(1.f, 1.f, 0.f, 1.f),
				16.f, kuto::Font::TYPE_NORMAL);
}


