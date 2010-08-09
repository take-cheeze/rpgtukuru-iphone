/**
 * @file
 * @brief Virtual Pad
 * @author project.kuto
 */
#pragma once

#include <cstring>
#include "kuto_types.h"
#include "kuto_math.h"
#include "kuto_task_singleton.h"
#include "kuto_irender.h"


namespace kuto {

class VirtualPad : public TaskSingleton<VirtualPad>, public IRender
{
	friend class TaskSingleton<VirtualPad>;
public:
	enum KEYS {
		KEY_UP,
		KEY_DOWN,
		KEY_LEFT,
		KEY_RIGHT,
		KEY_A,
		KEY_B,
		KEY_X,
		KEY_Y,
		KEY_START,
		KEY_MAX
	};	// enum KEYS
	enum {
		CLICK_COUNT		= 15,
	};
	struct KeyFlag {
		bool	onFlag_				: 1;
		bool	pressFlag_			: 1;
		bool	releaseFlag_		: 1;
		bool	clickFlag_			: 1;
		bool	repeatFlag_			: 1;
		int		onCount_			: 12;

		KeyFlag()
		: onFlag_(false), pressFlag_(false), releaseFlag_(false), clickFlag_(false), repeatFlag_(false)
		, onCount_(0) {}
	};	// struct KeyFlag
	struct KeyLayout {
		kuto::Vector2		position_;
		kuto::Vector2		size_;

		KeyLayout() : position_(0.f, 0.f), size_(0.f, 0.f) {}
	};	// struct KeyLayout

private:
	VirtualPad(Task* parent);

public:
	bool on(KEYS key) const { return keyFlags_[key].onFlag_; }
	bool press(KEYS key) const { return keyFlags_[key].pressFlag_; }
	bool release(KEYS key) const { return keyFlags_[key].releaseFlag_; }
	bool click(KEYS key) const { return keyFlags_[key].clickFlag_; }
	bool repeat(KEYS key) const { return keyFlags_[key].repeatFlag_; }
	int onCount(KEYS key) const { return keyFlags_[key].onCount_; }

	void setKeyFlag(KEYS key, KeyFlag const& f) { keyFlags_[key] = f; }

	void setKeyLayout(const KeyLayout* layouts) { std::memcpy(keyLayouts_, layouts, sizeof(KeyLayout) * KEY_MAX); }

	virtual void update();
	virtual void draw();
	virtual void render();
private:
	KeyFlag			keyFlags_[KEY_MAX];
	KeyLayout		keyLayouts_[KEY_MAX];
};	// class VirtualPad

}	// namespace kuto
