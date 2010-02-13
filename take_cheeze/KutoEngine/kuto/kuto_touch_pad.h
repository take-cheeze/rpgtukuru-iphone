/**
 * @file
 * @brief Touch Pad
 * @author project.kuto
 */
#pragma once

#include "kuto_math.h"
#include "kuto_singleton.h"


namespace kuto {

class TouchInfo
{
public:
	TouchInfo() { initialize(); }
	void initialize() {
		onFlag_ = false;
		pressFlag_ = false;
		releaseFlag_ = false;
		clickFlag_ = false;
		doubleClickFlag_ = false;
		moveFlag_ = false;
		position_.set(0.0f, 0.0f);
		prevPosition_.set(0.0f, 0.0f);
	}
	
public:
	Vector2		position_;
	Vector2		prevPosition_;
	struct {
		bool	onFlag_				: 1;
		bool	pressFlag_			: 1;
		bool	releaseFlag_		: 1;
		bool	clickFlag_			: 1;
		bool	doubleClickFlag_	: 1;
		bool	moveFlag_			: 1;
	};
};	// class TouchInfo


class TouchPad : public Singleton<TouchPad>
{
friend class Singleton<TouchPad>;
public:
	enum {
		MAX_TOUCH	= 4,
	};
private:
	TouchPad();

public:
	bool on(int index) const { return info_[index].onFlag_; }
	bool press(int index) const { return info_[index].pressFlag_; }
	bool release(int index) const { return info_[index].releaseFlag_; }
	bool click(int index) const { return info_[index].clickFlag_; }
	bool doubleClick(int index) const { return info_[index].doubleClickFlag_; }
	bool move(int index) const { return info_[index].moveFlag_; }
	const kuto::Vector2& position(int index) const { return info_[index].position_; }
	const kuto::Vector2& prevPosition(int index) const { return info_[index].prevPosition_; }
	kuto::Vector2 moveOffset(int index) const { return info_[index].position_ - info_[index].prevPosition_; }

	void update();
	void setTouches(const TouchInfo* info, int size);
	
private:
	TouchInfo			info_[MAX_TOUCH];
	TouchInfo			nextInfo_[MAX_TOUCH];
	bool				nextInfoChanged_;
	int					nextInfoTouchSize_;
};	// class TouchPad

}	// namespace kuto
