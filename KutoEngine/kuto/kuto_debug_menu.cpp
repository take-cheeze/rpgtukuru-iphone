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

DebugMenu::DebugMenu(Task* parent)
: Task(parent)
, cursor_(0), scroll_(0)
{
	kuto::VirtualPad::instance()->pauseDraw(false);
}

bool DebugMenu::initialize()
{
	return true;
}

void DebugMenu::update()
{
	kuto::VirtualPad* virtualPad = kuto::VirtualPad::instance();
	if (virtualPad->repeat(kuto::VirtualPad::KEY_UP)) {
		cursor_--;
		if (cursor_ < 0) {
			cursor_ = (int)kuto::SectionManager::instance()->getSectionHandles().size() - 1;
			scroll_ = kuto::max(0, (int)kuto::SectionManager::instance()->getSectionHandles().size() - kMaxRowSize);
		} else {
			if (cursor_ - scroll_ < 0)
				scroll_--;
		}
	}
	if (virtualPad->repeat(kuto::VirtualPad::KEY_DOWN)) {
		cursor_++;
		if (cursor_ >= (int)kuto::SectionManager::instance()->getSectionHandles().size()) {
			cursor_ = 0;
			scroll_ = 0;
		} else {
			if (cursor_ - scroll_ >= kMaxRowSize)
				scroll_++;
		}
	}
	if (virtualPad->repeat(kuto::VirtualPad::KEY_A)) {
		release();
		kuto::SectionManager::instance()->beginSection(kuto::SectionManager::instance()->getSectionHandles()[cursor_]->getName().c_str());
	}
}

void DebugMenu::draw()
{
	kuto::RenderManager::instance()->addRender(this, kuto::LAYER_2D_OBJECT, 0.f);
}

void DebugMenu::render()
{
	kuto::Graphics2D* g = kuto::RenderManager::instance()->getGraphics2D();

	g->drawText("Debug Menu", kuto::Vector2(5.f, 0.f), kuto::Color(1.f, 1.f, 1.f, 1.f),
		28.f, kuto::Font::NORMAL);
	
	u32 maxSize = kuto::min((u32)kMaxRowSize, (u32)kuto::SectionManager::instance()->getSectionHandles().size());
	for (u32 i = 0; i < maxSize; i++) {
		int index = i + scroll_;
		const char* str = kuto::SectionManager::instance()->getSectionHandles()[index]->getName().c_str();
		g->drawText(str, kuto::Vector2(20.f, 20.f * i + 30.f), cursor_ == index? kuto::Color(1.f, 1.f, 0.f, 1.f) : kuto::Color(1.f, 1.f, 1.f, 1.f),
			20.f, kuto::Font::NORMAL);
	}
	
	if (kMaxRowSize < kuto::SectionManager::instance()->getSectionHandles().size()) {
		// draw scroll bar
		kuto::Vector2 size(8.f, 210.f * (float)kMaxRowSize / (float)kuto::SectionManager::instance()->getSectionHandles().size());
		kuto::Vector2 pos(320.f - size.x, 30.f + scroll_ * (210.f - size.y) / (kuto::SectionManager::instance()->getSectionHandles().size() - kMaxRowSize));
		g->fillRectangle(pos, size, kuto::Color(0.8f, 0.8f, 0.8f, 1.f));
	}
}

}	// namespace kuto

