/**
 * @file
 * @brief Touch Pad
 * @author project.kuto
 */

#include "kuto_touch_pad.h"
#include "kuto_utility.h"


namespace kuto {

TouchPad::TouchPad()
: nextInfoChanged_(false), nextInfoTouchSize_(0)
{
}

void TouchPad::update()
{
	bool released[MAX_TOUCH];
	for (int i = 0; i < MAX_TOUCH; i++) {
		info_[i] = nextInfo_[i];
		released[i] = nextInfo_[i].releaseFlag_;
		
		nextInfo_[i].pressFlag_ = false;
		nextInfo_[i].releaseFlag_ = false;
		nextInfo_[i].clickFlag_ = false;
		nextInfo_[i].doubleClickFlag_ = false;
		nextInfo_[i].moveFlag_ = false;
		nextInfo_[i].prevPosition_ = nextInfo_[i].position_;
	}
	
	int tempMax = MAX_TOUCH;
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

void TouchPad::setTouches(const TouchInfo* info, int size)
{
	size = min<int>(MAX_TOUCH, size);
	
	if (nextInfoTouchSize_ > size) {
		int tempMax = size;
		for (int i = 0; i < tempMax; i++) {
			
		}
	}
	
	for (int i = 0; i < MAX_TOUCH; i++) {
		if (i < size) {
			if (nextInfoChanged_) {
				TouchInfo tempInfo(info[i]);
				if (nextInfo_[i].pressFlag_ && !nextInfo_[i].releaseFlag_) {
					tempInfo.pressFlag_ = true;
				}
				if (nextInfo_[i].releaseFlag_) {
					tempInfo.releaseFlag_ = true;
				}
				if (nextInfo_[i].clickFlag_) {
					tempInfo.clickFlag_ = true;
				}
				if (nextInfo_[i].doubleClickFlag_) {
					tempInfo.doubleClickFlag_ = true;
				}
				if (nextInfo_[i].moveFlag_) {
					tempInfo.moveFlag_ = true;
					tempInfo.prevPosition_ = nextInfo_[i].prevPosition_;
				}
				nextInfo_[i] = tempInfo;
			} else
				nextInfo_[i] = info[i];
			if (nextInfoTouchSize_ > size) {
				nextInfo_[i].moveFlag_ = false;
				nextInfo_[i].prevPosition_ = nextInfo_[i].position_;
			}
		} else {
			nextInfo_[i].initialize();
		}
	}
	nextInfoChanged_ = true;
	nextInfoTouchSize_ = size;
}

}	// namespace kuto

