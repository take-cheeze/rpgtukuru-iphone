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


TestFont::TestFont()
: kuto::IRender2D(kuto::Layer::OBJECT_2D, 0.f)
{
	kuto::VirtualPad::instance().pauseDraw(true);
}

bool TestFont::initialize()
{
	return isInitializedChildren();
}

void TestFont::update()
{
	if (kuto::TouchPad::instance().doubleClick(0)) {
		// return debug menu
		this->release();
	}
}

void TestFont::render(kuto::Graphics2D& g) const
{
	g.drawText("jypqあ", kuto::Vector2(0.f, 100.f), kuto::Color(1.f, 1.f, 1.f, 1.f),
		28.f, kuto::Font::NORMAL);
	g.drawText("Double Tap here to return.", kuto::Vector2(0.f, 150.f), kuto::Color(1.f, 1.f, 0.f, 1.f),
				16.f, kuto::Font::NORMAL);
	g.drawText("Good luck!", kuto::Vector2(0.f, 170.f), kuto::Color(1.f, 1.f, 0.f, 1.f),
				16.f, kuto::Font::NORMAL);
}
