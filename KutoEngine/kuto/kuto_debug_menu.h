/**
 * @file
 * @brief Debug Menu
 * @author project.kuto
 */
#pragma once

#include "kuto_task.h"
#include "kuto_irender.h"


namespace kuto {

class DebugMenu : public Task, public IRender
{
public:
	enum {
		kMaxRowSize = 10,
	};
	static Task* createTask(Task* parent) { return new DebugMenu(parent); }

public:
	int cursor() const { return cursor_; }
	virtual void render();

private:
	DebugMenu(Task* parent);

	virtual bool initialize();
	virtual void update();
	virtual void draw();

private:
	int			cursor_;
	int			scroll_;
};

}	// namespace kuto

