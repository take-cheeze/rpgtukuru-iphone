/**
 * @file
 * @brief Virtual Pad
 * @author project.kuto
 */

#include "kuto_virtual_pad.h"
#include "kuto_touch_pad.h"
#include "kuto_key_pad.h"
#include "kuto_error.h"
#include "kuto_render_manager.h"
#include "kuto_graphics2d.h"

#include "AppMain.h"


namespace kuto {

VirtualPad* VirtualPad::instance()
{
	return GetAppMain()->virtualPad();
}

VirtualPad::VirtualPad()
: IRender2D(kuto::Layer::OBJECT_2D, 0.f)
{
	keyLayouts_[KEY_UP].position_.set(50.f, 300.f);
	keyLayouts_[KEY_DOWN].position_.set(50.f, 380.f);
	keyLayouts_[KEY_LEFT].position_.set(10.f, 340.f);
	keyLayouts_[KEY_RIGHT].position_.set(90.f, 340.f);
	keyLayouts_[KEY_X].position_.set(230.f, 300.f); // Å™
	keyLayouts_[KEY_A].position_.set(270.f, 340.f); // Å®
	keyLayouts_[KEY_Y].position_.set(190.f, 340.f); // Å©
	keyLayouts_[KEY_B].position_.set(230.f, 380.f); // Å´
	keyLayouts_[KEY_START].position_.set(140.f, 440.f); // START

	for (int key = 0; key < KEY_MAX; key++)
		keyLayouts_[key].size_.set(40.f, 40.f);
}

void VirtualPad::update()
{
	TouchPad* touchPad = TouchPad::instance();
	KeyPad* keyPad = KeyPad::instance();

	#if !RPG2K_IS_PSP
		touchPad->update();
	#endif
	keyPad->update();

	KeyFlag oldFlag[KEY_MAX];
	for (int key = 0; key < KEY_MAX; key++) {
		oldFlag[key] = keyFlags_[key];
		keyFlags_[key].onFlag_ = false;
		keyFlags_[key].pressFlag_ = false;
		keyFlags_[key].releaseFlag_ = false;
		keyFlags_[key].clickFlag_ = false;
		keyFlags_[key].repeatFlag_ = false;
	}

	for (int i = 0; i < TouchPad::MAX_TOUCH; i++) {
		if (touchPad->on(i)) {
			kuto::Vector2 pos = touchPad->position(i);
			for (int key = 0; key < KEY_MAX; key++) {
				if (pos.x >= keyLayouts_[key].position_.x && pos.x <= keyLayouts_[key].position_.x + keyLayouts_[key].size_.x
				&& pos.y >= keyLayouts_[key].position_.y && pos.y <= keyLayouts_[key].position_.y + keyLayouts_[key].size_.y)
				{
					keyFlags_[key].onFlag_ = true;
					break;
				}
			}
		}
	}
	for (int i = 0; i < KeyPad::MAX_KEY; i++) {
		if (keyPad->on(i) && keyPad->key(i) != KEY_MAX) {
			keyFlags_[keyPad->key(i)].onFlag_ = true;
		}
	}
	for (int key = 0; key < KEY_MAX; key++) {
		if (keyFlags_[key].onFlag_) {
			if (!oldFlag[key].onFlag_) {
				keyFlags_[key].pressFlag_ = true;
				keyFlags_[key].repeatFlag_ = true;
			}
			if (keyFlags_[key].onCount_ > 15 && (keyFlags_[key].onCount_ % 4) == 0)
				keyFlags_[key].repeatFlag_ = true;
			keyFlags_[key].onCount_++;
		} else {
			if (oldFlag[key].onFlag_) {
				keyFlags_[key].releaseFlag_ = true;
				if (keyFlags_[key].onCount_ <= CLICK_COUNT)
					keyFlags_[key].clickFlag_ = true;
			}
			keyFlags_[key].onCount_ = 0;
		}
	}
}

void VirtualPad::render(kuto::Graphics2D* g) const
{
	kuto::Color color(1.f, 1.f, 1.f, 1.f);
	kuto::Color selectColor(1.f, 1.f, 0.f, 1.f);

	for (int key = 0; key < KEY_MAX; key++) {
		g->fillRectangle(keyLayouts_[key].position_, keyLayouts_[key].size_, keyFlags_[key].onFlag_? selectColor : color);
	}
}

}	// namespace kuto
