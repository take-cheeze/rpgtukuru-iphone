/**
 * @file
 * @brief Debug Menu
 * @author project.kuto
 */

#include "kuto_debug_menu.h"
#include "kuto_virtual_pad.h"
#include "kuto_section_manager.h"
#include "kuto_render_manager.h"
#include "kuto_graphics2d.h"
#include "kuto_utility.h"


namespace kuto {

DebugMenu::DebugMenu()
: IRender2D(kuto::Layer::OBJECT_2D, 0.f)
, cursor_(0), scroll_(0)
{
	kuto::VirtualPad::instance().pauseDraw(false);
}

bool DebugMenu::initialize()
{
	return true;
}

void DebugMenu::update()
{
	kuto::VirtualPad& virtualPad = kuto::VirtualPad::instance();
	if (virtualPad.repeat(kuto::VirtualPad::KEY_UP)) {
		cursor_--;
		if (cursor_ < 0) {
			cursor_ = (int)SectionManager::instance().sectionHandles().size() - 1;
			scroll_ = kuto::max(0, (int)SectionManager::instance().sectionHandles().size() - kMaxRowSize);
		} else {
			if (cursor_ - scroll_ < 0)
				scroll_--;
		}
	}
	if (virtualPad.repeat(kuto::VirtualPad::KEY_DOWN)) {
		cursor_++;
		if (cursor_ >= (int)SectionManager::instance().sectionHandles().size()) {
			cursor_ = 0;
			scroll_ = 0;
		} else {
			if (cursor_ - scroll_ >= kMaxRowSize)
				scroll_++;
		}
	}
	if (virtualPad.repeat(kuto::VirtualPad::KEY_A)) {
		release();
		SectionManager::instance().beginSection(SectionManager::instance().sectionHandles()[cursor_]->name().c_str());
	}
}

void DebugMenu::render(kuto::Graphics2D& g) const
{
	g.drawText("Debug Menu", kuto::Vector2(5.f, 0.f), kuto::Color(1.f, 1.f, 1.f, 1.f),
		28.f, kuto::Font::NORMAL);

	u32 maxSize = kuto::min((u32)kMaxRowSize, (u32)SectionManager::instance().sectionHandles().size());
	for (u32 i = 0; i < maxSize; i++) {
		int index = i + scroll_;
		std::string const& str = SectionManager::instance().sectionHandles()[index]->name();

		if( str == "Debug Menu" ) continue; // TODO: compare with "this" value

		g.drawText(str.c_str(), kuto::Vector2(20.f, 20.f * i + 30.f), cursor_ == index? kuto::Color(1.f, 1.f, 0.f, 1.f) : kuto::Color(1.f, 1.f, 1.f, 1.f),
			20.f, kuto::Font::NORMAL);
	}

	if (kMaxRowSize < SectionManager::instance().sectionHandles().size()) {
		// draw scroll bar
		kuto::Vector2 size(8.f, 210.f * (float)kMaxRowSize / (float)SectionManager::instance().sectionHandles().size());
		kuto::Vector2 pos(320.f - size.x, 30.f + scroll_ * (210.f - size.y) / (SectionManager::instance().sectionHandles().size() - kMaxRowSize));
		g.fillRectangle(pos, size, kuto::Color(0.8f, 0.8f, 0.8f, 1.f));
	}
}

}	// namespace kuto
