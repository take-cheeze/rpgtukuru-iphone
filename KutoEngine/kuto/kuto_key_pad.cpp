/**
 * @file
 * @brief Touch Pad
 * @author project.kuto
 */

#include "kuto_key_pad.h"
#include "kuto_utility.h"


namespace kuto {

KeyPad::KeyPad()
: nextInfoChanged_(false), nextInfoTouchSize_(0)
{
}

void KeyPad::update()
{
	bool released[MAX_KEY];
	for (int i = 0; i < MAX_KEY; i++) {
		info_[i] = nextInfo_[i];
		released[i] = nextInfo_[i].releaseFlag_;

		nextInfo_[i].pressFlag_ = false;
		nextInfo_[i].releaseFlag_ = false;
	}

	int tempMax = MAX_KEY;
	for (int i = 0; i < tempMax; i++) {
		if (released[i]) {
			nextInfo_[i] = nextInfo_[i+1];
			released[i] = released[i+1];
			tempMax--;
			i--;
		}
	}
	nextInfoChanged_ = false;
}

void KeyPad::setKeys(const KeyInfo* info, int size)
{
	size = min<int>(MAX_KEY, size);

	for (int i = 0; i < MAX_KEY; i++) {
		if (i < size) {
			if (nextInfoChanged_) {
				KeyInfo tempInfo(info[i]);
				if (nextInfo_[i].pressFlag_ && !nextInfo_[i].releaseFlag_) {
					tempInfo.pressFlag_ = true;
				}
				if (nextInfo_[i].releaseFlag_) {
					tempInfo.releaseFlag_ = true;
				}
				nextInfo_[i] = tempInfo;
			} else nextInfo_[i] = info[i];
		} else nextInfo_[i].reset();
	}
	nextInfoChanged_ = true;
	nextInfoTouchSize_ = size;
}

}	// namespace kuto
